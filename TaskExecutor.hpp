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

  void setTasks(const QList<Task> &newTasks);
  void addTask(const QString &command, int intervalMs);
  void clearTasks();

  void start();
  void stop();

  bool isRunning() const;

signals:
  void taskExecuted(const QString &command);
  void finished();

private slots:
  void executeNextTask();

private:
  QList<Task> tasks;
  QTimer timer;
  int currentIndex;
  bool running;
};

#endif // TASKEXECUTOR_H
