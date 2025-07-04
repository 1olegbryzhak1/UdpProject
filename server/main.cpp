#include <QCoreApplication>
#include <QSettings>
#include <QDir>
#include "server.h"
#include "../common/logger.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    Logger::minLogLevel = LogLevel::DEBUG;

    if (!QDir().mkpath("logs"))
    {
        Logger::write(LogLevel::ERROR, "Failed to create logs directory");
        return -1;
    }

    Logger::setLogFileName("logs/server.log");

    const QString configPath = "config.ini";

    if (!QFile::exists(configPath)) {
        Logger::write(LogLevel::ERROR, "Config file '" + configPath + "' not found in current server directory");
        return -1;
    }

    QSettings config(configPath, QSettings::IniFormat);

    Logger::write(LogLevel::INFO, "Server starting...");

    quint16 port = config.value("server/port").toUInt();
    if (port == 0)
    {
        Logger::write(LogLevel::ERROR, "Invalid port number");
        return -1;
    }

    static Server server;
    if (!server.start(port)) {
        return -1;
    }

    return app.exec();
}
