#ifndef GLOBALDEFINITION_HPP
#define GLOBALDEFINITION_HPP

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

#endif // GLOBALDEFINITION_HPP
