//
// Created by pengx on 2025/5/6.
//

#ifndef UTILS_H
#define UTILS_H

#include <QByteArray>
#include <QString>

class Utils {
public:
  Utils() = default;

  static QString formatByteArray(const QByteArray &data);

  static QByteArray formatHexString(const QString &command);

  static bool isHexString(const QString &command);

  static bool isPositiveInt(const QString &str);

  static uint8_t calculateCRC8(const QByteArray &data);

  static uint16_t calculateCRC16(const QByteArray &data);

  static uint8_t calculateXOR(const QByteArray &data);

  static uint8_t calculateChecksum(const QByteArray &data);
};

#endif // UTILS_H
