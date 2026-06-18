#include "DatabaseWrapper.hpp"
#include "Logger.hpp"
#include "qvariant.h"

#include <QSqlError>
#include <QSqlQuery>

DatabaseWrapper *DatabaseWrapper::_instancePtr = nullptr;

DatabaseWrapper::DatabaseWrapper() : _queryPtr{nullptr} {}

DatabaseWrapper::~DatabaseWrapper() {
  if (_queryPtr) {
    delete _queryPtr;
  }
}

DatabaseWrapper *DatabaseWrapper::get() {
  if (!_instancePtr) {
    _instancePtr = new DatabaseWrapper();
  }
  return _instancePtr;
}

bool DatabaseWrapper::init() {
  _db = QSqlDatabase::addDatabase("QSQLITE");
  _db.setDatabaseName("commands.db");

  if (!_db.open()) {
    Logger::Tag("DatabaseWrapper")
        .eFmt("数据库打开失败：%s",
              _db.lastError().text().toStdString().c_str());
    return false;
  }

  _queryPtr = new QSqlQuery(_db);

  if (!_queryPtr->exec("CREATE TABLE IF NOT EXISTS commands ("
                       "id INTEGER PRIMARY KEY, "
                       "value TEXT UNIQUE, "
                       "remark TEXT)")) {
    Logger::Tag("DatabaseWrapper")
        .eFmt("表创建失败：%s",
              _queryPtr->lastError().text().toStdString().c_str());
    return false;
  }

  initDefaultCommands();
  return true;
}

void DatabaseWrapper::initDefaultCommands() {
  _queryPtr->exec("SELECT COUNT(*) FROM commands");
  _queryPtr->next();
  int count = _queryPtr->value(0).toInt();

  if (count == 0) {
    QList<DefaultCommand> defaults = {
        {"FE FE 02 10 FA", "上电"},
        {"FE FE 02 13 FA", "下电"},
        {"FE FE 02 3D FA", "电位查询"},
        {"FE FE 0F 3C 08 00 08 00 08 00 08 00 08 00 08 00 14 FA", "复位"},
        {"FE FE 03 12 01 FA", "状态检测"}};

    _queryPtr->prepare("INSERT INTO commands (value, remark) VALUES (?, ?)");
    for (const auto &cmd : defaults) {
      _queryPtr->addBindValue(cmd.value);
      _queryPtr->addBindValue(cmd.remark);
      if (!_queryPtr->exec()) {
        Logger::Tag("DatabaseWrapper")
            .eFmt("插入默认指令失败：%s",
                  _queryPtr->lastError().text().toStdString().c_str());
      }
    }
  }
}

void DatabaseWrapper::addCommand(const QString &command,
                                 const QString &remark) {
  _queryPtr->prepare("INSERT INTO commands (value, remark) VALUES (?, ?)");
  _queryPtr->addBindValue(command);
  _queryPtr->addBindValue(remark);

  if (!_queryPtr->exec()) {
    Logger::Tag("DatabaseWrapper")
        .eFmt("插入失败：%s",
              _queryPtr->lastError().text().toStdString().c_str());
  }
}

void DatabaseWrapper::updateCommand(int id, const QString &command,
                                    const QString &remark) {
  _queryPtr->prepare("UPDATE commands SET value = ?, remark = ? WHERE id = ?");
  _queryPtr->addBindValue(command);
  _queryPtr->addBindValue(remark);
  _queryPtr->addBindValue(id);

  if (!_queryPtr->exec()) {
    Logger::Tag("DatabaseWrapper")
        .eFmt("更新失败：%s",
              _queryPtr->lastError().text().toStdString().c_str());
  }
}

void DatabaseWrapper::deleteCommand(int id) {
  _queryPtr->prepare("DELETE FROM commands WHERE id = ?");
  _queryPtr->addBindValue(id);

  if (!_queryPtr->exec()) {
    Logger::Tag("DatabaseWrapper")
        .eFmt("删除失败：%s",
              _queryPtr->lastError().text().toStdString().c_str());
  }
}

QList<DatabaseCommand> DatabaseWrapper::getAllCommands() {
  QList<DatabaseCommand> commands;

  if (!_queryPtr->exec("SELECT id, value, remark FROM commands")) {
    Logger::Tag("DatabaseWrapper")
        .e(_queryPtr->lastError().text().toStdString().c_str());
    return commands;
  }

  while (_queryPtr->next()) {
    DatabaseCommand cmd;
    cmd.id = _queryPtr->value(0).toInt();
    cmd.value = _queryPtr->value(1).toString();
    cmd.remark = _queryPtr->value(2).toString();
    commands.append(cmd);
  }

  return commands;
}

bool DatabaseWrapper::commandExists(const QString &command) {
  _queryPtr->prepare("SELECT COUNT(*) FROM commands WHERE value = ?");
  _queryPtr->addBindValue(command);

  if (!_queryPtr->exec()) {
    return false;
  }

  if (_queryPtr->next()) {
    return _queryPtr->value(0).toInt() > 0;
  }

  return false;
}