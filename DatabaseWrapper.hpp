#ifndef DATABASEWRAPPER_HPP
#define DATABASEWRAPPER_HPP

#include "GlobalDefinition.hpp"
#include "Logger.hpp"

#include <QSqlDatabase>
#include <QString>

class DatabaseWrapper {
public:
  static DatabaseWrapper *get();

  bool init();
  void addCommand(const QString &command, const QString &remark);
  void updateCommand(int id, const QString &command, const QString &remark);
  void deleteCommand(int id);
  QList<DatabaseCommand> getAllCommands();
  bool commandExists(const QString &command);

private:
  DatabaseWrapper();
  ~DatabaseWrapper();

  static DatabaseWrapper *_instancePtr;
  Logger _logger;
  QSqlDatabase _db;
  QSqlQuery *_queryPtr;

  void initDefaultCommands();
};

#endif // DATABASEWRAPPER_HPP
