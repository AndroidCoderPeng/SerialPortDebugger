//
// Created by Administrator on 2025/5/2.
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QListWidgetItem>
#include <QMainWindow>

#include "Logger.hpp"
#include "TaskExecutor.hpp"

QT_BEGIN_NAMESPACE

namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QMainWindow *parent = nullptr);

  ~MainWindow() override;

public slots:
  void slotDataReceived(const QByteArray &data);

private:
  Ui::MainWindow *ui;
  Logger _logger;
  TaskExecutor *executorPtr = nullptr;

#ifdef Q_OS_LINUX
  QListWidgetItem *previousSelectedItemPtr = nullptr;
#endif

  QList<PortMessage> history;
  QTimer *timerPtr = nullptr;

  void updateComboxState(bool disabled) const;

  void onOpenPortButtonClicked();

  void onRefreshButtonClicked();

  void onSaveDataButtonClicked();

  void onClearDataButtonClicked();

  void onAddCommandButtonClicked();

  void onCommandItemClicked(const QListWidgetItem *item);

  void updateCommandWidget(const qint16 &id, const QString &command,
                           const QString &remark);

  void showCommandWidgetContextMenu(const QPoint &pos);

  void onCustomAction(const QListWidgetItem *item, const QString &message);

  void onSendCommandButtonClicked();

  void sendCommand(const QString &command);

  void updateComMessageLog(const QByteArray &data, const QString &direction);

  void onScriptButtonClicked();

  void onTimeCheckBoxStateChanged(const qint16 &state);

  void uncheckTimeCheckBox();

  void onEncodeCheckBoxStateChanged(const qint16 &state);

private slots:
  void executeTask(const QString &command);
  void onScriptFinished();
};

#endif // MAINWINDOW_H
