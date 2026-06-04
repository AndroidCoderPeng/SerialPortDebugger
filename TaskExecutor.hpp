#ifndef TASKEXECUTOR_H
#define TASKEXECUTOR_H

#include <QList>
#include <QObject>
#include <QString>
#include <QTimer>

#include "GlobalDefinition.hpp"

class TaskExecutor : public QObject {
  Q_OBJECT
public:
  explicit TaskExecutor(QObject *parent = nullptr);
  void addTask(const QString &command, int interval);
  void start();

signals:
  void taskExecuted(const QString &command); // 通知任务执行

private slots:
  void executeNextTask();

private:
  QList<Task> tasks;
  QTimer timer;
  int currentIndex;
};

#endif // TASKEXECUTOR_H
