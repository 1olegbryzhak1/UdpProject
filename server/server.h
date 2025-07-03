#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QUdpSocket>

class Server : public QObject {
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);
    bool start(quint16 port);

private slots:
    void handleReadyRead();

private:
    QUdpSocket udpSocket;
};
#endif // SERVER_H
