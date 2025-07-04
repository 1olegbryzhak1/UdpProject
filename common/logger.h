#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>

enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3
};

class Logger {
public:
    static inline auto minLogLevel = LogLevel::DEBUG;
    static inline QString logFileName = "app.log";

    static void setLogFileName(const QString& name) {
        logFileName = name;
    }

    static void write(LogLevel level, const QString &msg) {
        if (level < minLogLevel)
            return;

        QString timeStamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        QString prefix;

        switch (level) {
        case LogLevel::DEBUG:   prefix = "[DEBUG] "; break;
        case LogLevel::INFO:    prefix = "[INFO] "; break;
        case LogLevel::WARNING: prefix = "[WARN] "; break;
        case LogLevel::ERROR:   prefix = "[ERROR] "; break;
        }

        const QString fullMessage = QString("%1%2%3").arg(timeStamp, prefix, msg);

        QFile file(logFileName);
        if (file.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&file);
            out << fullMessage << "\n";
            out.flush();
            file.close();
        }

        qDebug().noquote() << fullMessage;
    }

    static LogLevel fromString(const QString &levelStr) {
        QString l = levelStr.trimmed().toLower();
        if (l == "debug") return LogLevel::DEBUG;
        if (l == "info") return LogLevel::INFO;
        if (l == "warn" || l == "warning") return LogLevel::WARNING;
        if (l == "error") return LogLevel::ERROR;
        return LogLevel::INFO;
    }
};

#endif // LOGGER_H
