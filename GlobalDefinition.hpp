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

#endif // GLOBALDEFINITION_HPP
