#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QtGlobal>

constexpr quint8 PROTOCOL_VERSION = 2;
constexpr int DOUBLE_COUNT = 1000000;

enum class MessageType : quint8 {
    REQUEST_RANDOM_ARRAY = 1,
    RESPONSE_DATA = 2,
    ERROR_UNSUPPORTED_VERSION = 3
};

struct ProtocolHeader {
    quint8 version;
    quint8 messageType;
    quint16 chunkId;
    quint32 payloadSize;
    quint16 totalChunks;
    quint32 crc32;
    quint16 reserved;
};

inline QByteArray createRequestPacket(double value) {
    ProtocolHeader header{};
    header.version = PROTOCOL_VERSION;
    header.messageType = static_cast<quint8>(MessageType::REQUEST_RANDOM_ARRAY);
    header.payloadSize = sizeof(double);

    QByteArray packet;
    packet.append(reinterpret_cast<const char*>(&header), sizeof(header));
    packet.append(reinterpret_cast<const char*>(&value), sizeof(double));
    return packet;
}


#endif // PROTOCOL_H
