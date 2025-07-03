#include <QCoreApplication>
#include <QSettings>
#include "server.h"
#include "../common/logger.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    QSettings config("config.ini", QSettings::IniFormat);
    Logger::minLogLevel = Logger::fromString(config.value("log/level", "debug").toString());
    Logger::write(LogLevel::INFO, "Server starting...");

    quint16 port = config.value("server/port", 45454).toUInt();
    if (Server server; !server.start(port))
    {
        return -1;
    }

    return app.exec();
}
