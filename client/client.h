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
    quint16 serverPort = 0;
    double requestValue = 0;

    QVector<double> receivedData;
    QSet<int> receivedChunks;
    int totalChunks = -1;

    void loadConfig();
    void writeToFile(const QByteArray &data);
    void writeToTextFile(const QVector<double> &data);
    void logError(const QString &msg);
};

#endif // CLIENT_H
