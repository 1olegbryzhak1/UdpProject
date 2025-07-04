#include "server.h"
#include "../common/protocol.h"
#include "../common/logger.h"
#include <QRandomGenerator>
#include <QSet>
#include <thread>
#include <chrono>

Server::Server(QObject *parent) : QObject(parent) {}

bool Server::start(quint16 port) {
    if (!udpSocket.bind(QHostAddress::LocalHost, port)) {
        Logger::write(LogLevel::ERROR, QString("Server failed to bind port %1").arg(port));
        return false;
    }
    connect(&udpSocket, &QUdpSocket::readyRead, this, &Server::handleReadyRead);
    Logger::write(LogLevel::INFO, QString("Server started on port %1").arg(port));
    return true;
}

void Server::handleReadyRead() {
    while (udpSocket.hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket.pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        udpSocket.readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        if (datagram.size() < sizeof(ProtocolHeader)) {
            Logger::write(LogLevel::WARNING, "Received too small datagram");
            continue;
        }

        const auto* header = reinterpret_cast<const ProtocolHeader*>(datagram.constData());
        if (header->version != PROTOCOL_VERSION) {
            Logger::write(LogLevel::ERROR, "Unsupported protocol version");
            ProtocolHeader errorHeader{PROTOCOL_VERSION, static_cast<quint8>(MessageType::ERROR_UNSUPPORTED_VERSION), 0, 0, 0, 0};
            udpSocket.writeDatagram(reinterpret_cast<const char*>(&errorHeader), sizeof(errorHeader), sender, senderPort);
            continue;
        }

        if (header->messageType == static_cast<quint8>(MessageType::REQUEST_RANDOM_ARRAY)) {
            double x;
            memcpy(&x, datagram.constData() + sizeof(ProtocolHeader), sizeof(double));
            Logger::write(LogLevel::INFO, QString("Generating %1 doubles in range [-%2, %2]").arg(DOUBLE_COUNT).arg(x));

            QVector<double> data;
            QSet<double> uniqueSet;
            while (data.size() < DOUBLE_COUNT) {
                double val = QRandomGenerator::global()->generateDouble() * (2.0 * x) - x;
                if (!uniqueSet.contains(val)) {
                    uniqueSet.insert(val);
                    data.append(val);
                }
            }

            int chunkSize = 5000; // about 40KB
            int totalChunks = (data.size() + chunkSize - 1) / chunkSize;

            for (int i = 0; i < totalChunks; ++i) {
                int start = i * chunkSize;
                int count = std::min<int>(chunkSize, data.size() - start);

                QByteArray payload(reinterpret_cast<const char*>(data.data() + start), count * sizeof(double));

                ProtocolHeader chunkHeader;
                chunkHeader.version = PROTOCOL_VERSION;
                chunkHeader.messageType = static_cast<quint8>(MessageType::RESPONSE_DATA);
                chunkHeader.chunkId = i;
                chunkHeader.totalChunks = totalChunks;
                chunkHeader.payloadSize = payload.size();
                chunkHeader.crc32 = qChecksum(payload.constData());
                chunkHeader.reserved = 0;

                QByteArray packet;
                packet.append(reinterpret_cast<const char*>(&chunkHeader), sizeof(chunkHeader));
                packet.append(payload);

                udpSocket.writeDatagram(packet, sender, senderPort);

                // delay for avoid missing packets
                std::this_thread::sleep_for(std::chrono::milliseconds(1));

                Logger::write(LogLevel::DEBUG, QString("Sent chunk %1/%2 (%3 doubles) to %4:%5")
                              .arg(i+1).arg(totalChunks).arg(count).arg(sender.toString()).arg(senderPort));
            }

            Logger::write(LogLevel::INFO, QString("Sent all %1 doubles in %2 chunks").arg(data.size()).arg(totalChunks));
        }
    }
}
