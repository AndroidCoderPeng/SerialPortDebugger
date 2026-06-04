#include "TaskExecutor.hpp"

TaskExecutor::TaskExecutor(QObject *parent)
    : QObject(parent), currentIndex(0), running(false) {
  timer.setSingleShot(true);
  connect(&timer, &QTimer::timeout, this, &TaskExecutor::executeNextTask);
}

void TaskExecutor::setTasks(const QList<Task> &newTasks) {
  if (running) {
    stop();
  }
  tasks = newTasks;
  currentIndex = 0;
}

void TaskExecutor::addTask(const QString &command, const int intervalMs) {
  Task task;
  task.command = command;
  task.interval = intervalMs > 0 ? intervalMs : 1; // 最小 1ms，避免异常值
  tasks.append(task);
}

void TaskExecutor::clearTasks() {
  if (running) {
    stop();
  }
  tasks.clear();
  currentIndex = 0;
}

void TaskExecutor::start() {
  if (running || tasks.isEmpty()) {
    return;
  }

  running = true;
  currentIndex = 0;
  executeNextTask(); // 第一个任务立即执行
}

void TaskExecutor::stop() {
  if (!running) {
    return;
  }

  timer.stop();
  running = false;
  currentIndex = 0;
  emit finished();
}

bool TaskExecutor::isRunning() const { return running; }

void TaskExecutor::executeNextTask() {
  if (!running) {
    return;
  }

  if (currentIndex >= tasks.size()) {
    timer.stop();
    running = false;
    emit finished();
    return;
  }

  const Task currentTask = tasks.at(currentIndex);
  emit taskExecuted(currentTask.command);

  currentIndex++;
  if (currentIndex < tasks.size()) {
    const int nextDelayMs = currentTask.interval > 0 ? currentTask.interval : 1;
    timer.start(nextDelayMs);
  } else {
    running = false;
    emit finished();
  }
}
