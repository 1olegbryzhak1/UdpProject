#include <QCoreApplication>
#include <QSettings>
#include <QDir>
#include <QRandomGenerator>
#include "client.h"
#include "../common/logger.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    Logger::minLogLevel = LogLevel::DEBUG;

    if (!QDir().mkpath("output"))
    {
        Logger::write(LogLevel::ERROR, "Failed to create output directory");
        return -1;
    }

    if (!QDir().mkpath("logs"))
    {
        Logger::write(LogLevel::ERROR, "Failed to create logs directory");
        return -1;
    }

    int clientId = QRandomGenerator::global()->bounded(100000);

    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss_zzz");
    Logger::setLogFileName(QString("logs/client_%1_%2.log").arg(clientId).arg(timestamp));

    auto *client = new Client(clientId, &app);
    client->start();

    // just for testing
    // QTimer timer;
    // QObject::connect(&timer, &QTimer::timeout, []() {
    //     Logger::write(LogLevel::INFO, "[Main thread is alive]");
    // });
    // timer.start(500);

    return app.exec();
}