#include "TaskExecutor.hpp"

TaskExecutor::TaskExecutor(QObject *parent) : QObject(parent), currentIndex(0) {
  connect(&timer, &QTimer::timeout, this, &TaskExecutor::executeNextTask);
}

void TaskExecutor::addTask(const QString &command, int interval) {
  tasks.append({command, interval});
}

void TaskExecutor::start() {
  if (!tasks.isEmpty()) {
    currentIndex = 0;
    executeNextTask(); // 触发第一个任务立即执行
  }
}

void TaskExecutor::executeNextTask() {
  if (currentIndex < tasks.size()) {
    const auto &currentTask = tasks[currentIndex];
    // 信号通知
    emit taskExecuted(currentTask.command);

    currentIndex++;
    timer.start(currentTask.interval);
  }
}
