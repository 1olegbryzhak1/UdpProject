#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QHostAddress>

class Client : public QObject {
    Q_OBJECT

public:
    explicit Client(QObject *parent = nullptr);
    void start();

private slots:
    void sendRequest();
    void handleResponse();

private:
    QUdpSocket udpSocket;
    QTimer timer;
    QHostAddress serverAddress;
    quint16 serverPort;
    double requestValue;

    void loadConfig();
    void writeToFile(const QByteArray &data);
    void logError(const QString &msg);
};

#endif // CLIENT_H
