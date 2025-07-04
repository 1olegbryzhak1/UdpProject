#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QHostAddress>
#include <QSet>

class Client : public QObject {
    Q_OBJECT

public:
    explicit Client(int id, QObject *parent = nullptr);
    void start();

private slots:
    void sendRequest();
    void handleResponse();
    void onReceiveTimeout();

private:
    QUdpSocket udpSocket;
    QTimer timer;
    QHostAddress serverAddress;
    quint16 serverPort = 0;
    double requestValue = 0;

    QVector<double> receivedData;
    QSet<int> receivedChunks;
    QTimer receiveTimeout;
    int totalChunks = -1;
    int clientId = -1;

    bool loadConfig();
    void writeToFile(const QVector<double> &data, const QString& filename);
    void writeToTextFile(const QVector<double> &data, const QString& filename);
};

#endif // CLIENT_H
