//
// Created by pengx on 2025/5/6.
//

#include "Utils.hpp"

#include <QDebug>
#include <QRegularExpression>
#include <QTextCodec>

QString Utils::formatByteArray(const QByteArray &data) {
  const QString hex = data.toHex().toUpper();
  QString hexWithSpaces;
  for (int i = 0; i < hex.length(); i += 2) {
    hexWithSpaces += hex.mid(i, 2) + " ";
  }

  if (!hexWithSpaces.isEmpty()) {
    hexWithSpaces.chop(1);
  }
  return hexWithSpaces;
}

QByteArray Utils::formatHexString(const QString &command) {
  QByteArray byteArray;
  QStringList hexList = command.split(' ', QString::SkipEmptyParts);
  // 使用 const 引用来避免复制
  const QStringList &listRef = hexList;
  for (const QString &hex : listRef) {
    bool ok;
    const uint value = hex.toUInt(&ok, 16);
    if (ok) {
      byteArray.append(static_cast<char>(value));
    }
  }
  return byteArray;
}

bool Utils::isHexString(const QString &command) {
  // 移除所有空格
  QString cleanedStr = command;
  cleanedStr.remove(' ');

  // 使用正则表达式检查是否为有效的十六进制字符串
  static const QRegularExpression hexPattern("^[0-9a-fA-F]+$");
  return hexPattern.match(cleanedStr).hasMatch();
}

bool Utils::isPositiveInt(const QString &str) {
  bool ok;
  const int value = str.toInt(&ok);
  return ok && value > 0;
}

uint8_t Utils::calculateCRC8(const QByteArray &data) {
  uint8_t crc = 0x00;
  for (const auto &byte : data) {
    crc ^= static_cast<uint8_t>(byte);
    for (int i = 0; i < 8; ++i) {
      if (crc & 0x80) {
        crc = (crc << 1) ^ 0x07;
      } else {
        crc <<= 1;
      }
    }
  }
  return crc;
}

uint16_t Utils::calculateCRC16(const QByteArray &data) {
  uint16_t crc = 0xFFFF;
  for (const auto &byte : data) {
    crc ^= static_cast<uint8_t>(byte);
    for (int i = 0; i < 8; ++i) {
      if (crc & 0x0001) {
        crc = (crc >> 1) ^ 0xA001;
      } else {
        crc >>= 1;
      }
    }
  }
  return crc;
}

uint8_t Utils::calculateXOR(const QByteArray &data) {
  uint8_t xorCode = 0x00;
  for (const auto &byte : data) {
    xorCode ^= static_cast<uint8_t>(byte);
  }
  return xorCode;
}

uint8_t Utils::calculateChecksum(const QByteArray &data) {
  uint8_t checksum = 0x00;
  for (const auto &byte : data) {
    checksum += static_cast<uint8_t>(byte);
  }
  return ~checksum; // 取反
}
