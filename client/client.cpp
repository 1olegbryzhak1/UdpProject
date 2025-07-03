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
    Logger::write(LogLevel::INFO, "Client socket bound");
    timer.setSingleShot(true);
    timer.start(3000);
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
    qint64 sent = udpSocket.writeDatagram(packet, serverAddress, serverPort);
    Logger::write(LogLevel::INFO, QString("Sent request (%1 bytes) to %2:%3")
                  .arg(sent).arg(serverAddress.toString()).arg(serverPort));
}

void Client::handleResponse() {
    Logger::write(LogLevel::DEBUG, "handleResponse() triggered");
    while (udpSocket.hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket.pendingDatagramSize());
        udpSocket.readDatagram(datagram.data(), datagram.size());

        Logger::write(LogLevel::DEBUG, QString("Received datagram size: %1").arg(datagram.size()));

        if (datagram.size() < sizeof(ProtocolHeader)) {
            logError("Datagram too small");
            return;
        }

        const ProtocolHeader *header = reinterpret_cast<const ProtocolHeader *>(datagram.constData());

        if (header->messageType == static_cast<quint8>(MessageType::ERROR_UNSUPPORTED_VERSION)) {
            logError("Unsupported protocol version from server");
            return;
        }

        if (header->messageType == static_cast<quint8>(MessageType::RESPONSE_DATA)) {
            QByteArray payload = datagram.mid(sizeof(ProtocolHeader));
            QVector<double> values(payload.size() / sizeof(double));
            memcpy(values.data(), payload.constData(), payload.size());
            std::sort(values.begin(), values.end(), std::greater<>());

            QByteArray sorted(reinterpret_cast<const char*>(values.data()), values.size() * sizeof(double));
            writeToFile(sorted);
            Logger::write(LogLevel::INFO, QString("Saved %1 sorted doubles to output.bin").arg(values.size()));
        }
    }
}

void Client::writeToFile(const QByteArray &data) {
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