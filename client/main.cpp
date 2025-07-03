#include <QCoreApplication>
#include <QSettings>
#include "client.h"
#include "../common/logger.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    QSettings config("config.ini", QSettings::IniFormat);
    Logger::minLogLevel = Logger::fromString(config.value("log/level", "debug").toString());
    Logger::write(LogLevel::INFO, "Client starting...");

    Client client;
    client.start();

    return app.exec();
}