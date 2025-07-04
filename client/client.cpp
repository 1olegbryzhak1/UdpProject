#include "client.h"
#include "../common/protocol.h"
#include "../common/logger.h"
#include <QSettings>
#include <QFile>
#include <QDataStream>
#include <algorithm>

Client::Client(QObject *parent) : QObject(parent) {
    connect(&udpSocket, &QUdpSocket::readyRead, this, &Client::handleResponse);
    connect(&timer, &QTimer::timeout, this, &Client::sendRequest);
}

void Client::start() {
    loadConfig();
    if (!udpSocket.bind()) {
        Logger::write(LogLevel::ERROR, "Client failed to bind socket");
        return;
    }

    udpSocket.setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, 1024 * 1024); // 1MB

    Logger::write(LogLevel::INFO, "Client socket bound");
    timer.setSingleShot(true);
    timer.start(3000);

    connect(&receiveTimeout, &QTimer::timeout, this, &Client::onReceiveTimeout);
    receiveTimeout.setSingleShot(true);
    receiveTimeout.start(5000);
}

void Client::loadConfig() {
    QSettings config("config.ini", QSettings::IniFormat);
    serverAddress = QHostAddress(config.value("client/host", "127.0.0.1").toString());
    serverPort = config.value("client/port", 45454).toUInt();
    requestValue = config.value("client/value", 100.0).toDouble();
    Logger::write(LogLevel::DEBUG, QString("Loaded config: host=%1, port=%2, value=%3")
                  .arg(serverAddress.toString()).arg(serverPort).arg(requestValue));
}

void Client::sendRequest() {
    const QByteArray packet = createRequestPacket(requestValue);

    Logger::write(LogLevel::INFO, QString("Sending request (%1 bytes) to %2:%3")
              .arg(packet.size()).arg(serverAddress.toString()).arg(serverPort));

    const qint64 sent = udpSocket.writeDatagram(packet, serverAddress, serverPort);

    Logger::write(LogLevel::INFO, QString("Sent request (%1 bytes) to %2:%3")
                  .arg(sent).arg(serverAddress.toString()).arg(serverPort));
}

void Client::handleResponse() {
    Logger::write(LogLevel::DEBUG, "handleResponse() triggered");

    while (udpSocket.hasPendingDatagrams())
    {
        Logger::write(LogLevel::DEBUG, "Reading datagram ...");

        QByteArray datagram;
        datagram.resize(udpSocket.pendingDatagramSize());
        udpSocket.readDatagram(datagram.data(), datagram.size());

        if (datagram.size() < sizeof(ProtocolHeader)) {
            Logger::write(LogLevel::ERROR, "Datagram too small");
            continue;
        }

        auto header = reinterpret_cast<const ProtocolHeader *>(datagram.constData());
        if (header->messageType != static_cast<quint8>(MessageType::RESPONSE_DATA)) {
            Logger::write(LogLevel::WARNING, "Unexpected message type received");
            continue;
        }

        if (totalChunks == -1)
        {
            totalChunks = header->totalChunks;
            Logger::write(LogLevel::DEBUG, "Total chunks - " + QString::number(totalChunks));
        }

        if (receivedChunks.contains(header->chunkId)) {
            Logger::write(LogLevel::DEBUG, QString("Chunk %1 already received").arg(header->chunkId));
            continue;
        }

        QByteArray payload = datagram.mid(sizeof(ProtocolHeader));
        int count = payload.size() / sizeof(double);
        QVector<double> chunk(count);
        memcpy(chunk.data(), payload.constData(), payload.size());

        receivedData += chunk;
        receivedChunks.insert(header->chunkId);

        Logger::write(LogLevel::DEBUG, QString("Received chunk %1 (%2 bytes, %3 doubles)")
                      .arg(header->chunkId + 1).arg(payload.size()).arg(count));

        if (receivedChunks.size() == totalChunks) {
            Logger::write(LogLevel::INFO, "All chunks received. Sorting...");
            std::sort(receivedData.begin(), receivedData.end(), std::greater<>());
            Logger::write(LogLevel::INFO, "Sorting finished");
            writeToTextFile(receivedData);
            Logger::write(LogLevel::INFO, QString("Saved %1 sorted doubles to output.txt").arg(receivedData.size()));
        }
    }
}

void Client::writeToTextFile(const QVector<double> &data) {
    Logger::write(LogLevel::INFO, "Saving data to textfile...");

    QFile file("output.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        Logger::write(LogLevel::ERROR, "Cannot open output.txt");
        return;
    }
    QTextStream out(&file);
    for (double val : data) {
        out << val << "\n";
    }
    file.close();
}

void Client::writeToFile(const QByteArray &data) {
    Logger::write(LogLevel::INFO, "Saving data to binary file...");

    QFile file("output.bin");
    if (!file.open(QIODevice::WriteOnly)) {
        Logger::write(LogLevel::ERROR, "Cannot open output.bin");
        return;
    }
    file.write(data);
    file.close();
}

void Client::logError(const QString &msg) {
    Logger::write(LogLevel::ERROR, msg);
    QFile file("client_error.log");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << msg << "\n";
        file.close();
    }
}

void Client::onReceiveTimeout() {
    if (totalChunks == -1) {
        Logger::write(LogLevel::ERROR, "Timeout: did not receive any chunks");
        return;
    }

    if (receivedChunks.size() < totalChunks) {
        QStringList missing;
        for (int i = 0; i < totalChunks; ++i) {
            if (!receivedChunks.contains(i)) {
                missing << QString::number(i);
            }
        }

        Logger::write(LogLevel::ERROR, QString("Missing %1/%2 chunks. IDs: [%3]")
                      .arg(totalChunks - receivedChunks.size())
                      .arg(totalChunks)
                      .arg(missing.join(", ")));
    } else {
        Logger::write(LogLevel::INFO, "All chunks received before timeout");
    }
}