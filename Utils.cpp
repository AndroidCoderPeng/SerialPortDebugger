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
