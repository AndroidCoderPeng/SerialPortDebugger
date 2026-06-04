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
};

#endif // UTILS_H
