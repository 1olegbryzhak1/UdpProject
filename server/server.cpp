#include "server.h"
#include "../common/protocol.h"
#include "../common/logger.h"
#include <QRandomGenerator>
#include <QSet>

Server::Server(QObject *parent) : QObject(parent) {}

bool Server::start(quint16 port) {
    if (!udpSocket.bind(QHostAddress::Any, port)) {
        Logger::write(LogLevel::ERROR, QString("Server failed to bind port %1").arg(port));
        return false;
    }
    connect(&udpSocket, &QUdpSocket::readyRead, this, &Server::handleReadyRead);
    Logger::write(LogLevel::INFO, QString("Server started on port %1").arg(port));
    return true;
}

void Server::handleReadyRead() {
    Logger::write(LogLevel::DEBUG, "handleReadyRead() triggered");
    while (udpSocket.hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket.pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        udpSocket.readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        Logger::write(LogLevel::DEBUG, QString("Received datagram from %1:%2 size=%3")
                      .arg(sender.toString()).arg(senderPort).arg(datagram.size()));

        if (datagram.size() < sizeof(ProtocolHeader)) {
            Logger::write(LogLevel::WARNING, "Received too small datagram");
            continue;
        }

        const auto* header = reinterpret_cast<const ProtocolHeader*>(datagram.data());
        if (header->version != PROTOCOL_VERSION) {
            Logger::write(LogLevel::ERROR, "Unsupported protocol version");
            ProtocolHeader errorHeader{PROTOCOL_VERSION, static_cast<quint8>(MessageType::ERROR_UNSUPPORTED_VERSION), 0, 0};
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

            QByteArray payload;
            payload.resize(sizeof(double) * data.size());
            memcpy(payload.data(), data.data(), payload.size());

            ProtocolHeader responseHeader{PROTOCOL_VERSION, static_cast<quint8>(MessageType::RESPONSE_DATA), 0, static_cast<quint32>(payload.size())};

            QByteArray packet;
            packet.append(reinterpret_cast<const char*>(&responseHeader), sizeof(responseHeader));
            packet.append(payload);

            udpSocket.writeDatagram(packet, sender, senderPort);
            Logger::write(LogLevel::INFO, QString("Sent %1 doubles to %2:%3")
                          .arg(data.size()).arg(sender.toString()).arg(senderPort));
        }
    }
}