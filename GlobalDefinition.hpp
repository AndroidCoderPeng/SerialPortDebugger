#ifndef GLOBALDEFINITION_HPP
#define GLOBALDEFINITION_HPP

#include <QDateTime>
#include <QString>

struct DefaultCommand {
  QString value;  // 指令值
  QString remark; // 指令备注
};

struct DatabaseCommand : public DefaultCommand {
  int id; // 数据库主键ID
};

struct Task {
  QString command;
  int interval = 1000; // 默认间隔1秒
};

struct ScriptConfig {
  QString command;
  QString remark;
  qint16 interval = 1000; // 默认间隔1秒
};

struct PortMessage {
  QByteArray data;       // 收发的数据内容
  QString direction;     // 方向："发" 或 "收"
  qint64 timestamp = 0;  // 时间戳（毫秒）
  QString formattedTime; // 格式化时间（如 "14:30:00.123"）

  PortMessage() = default;

  PortMessage(const QByteArray &d, const QString &dir, qint64 ts)
      : data(d), direction(dir), timestamp(ts),
        formattedTime(
            QDateTime::fromMSecsSinceEpoch(ts).toString("hh:mm:ss.zzz")) {}
};

#endif // GLOBALDEFINITION_HPP
