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

private:
  Ui::MainWindow *ui;
  Logger _logger;
  TaskExecutor *executorPtr = nullptr;
  QListWidgetItem *previousSelectedItemPtr = nullptr;

  QList<PortMessage> history;
  QTimer *timerPtr = nullptr;
  QList<QString> checkCodeTypes = {"无校验码",  "CRC-8", "CRC-16(L)",
                                   "CRC-16(H)", "XOR",   "Checksum"};
  static constexpr int kAddItemMagicId = -999; // 标记"添加新指令"占位项

  void updateComboxState(bool disabled) const;

  void onOpenPortButtonClicked();

  void onRefreshButtonClicked();

  void onSaveDataButtonClicked();

  void onClearDataButtonClicked();

  void refreshCommandList();

  void onCommandItemClicked(const QListWidgetItem *item);

  void onAddCommandItemClicked();

  void updateCommandWidget(const qint16 &id, const QString &command,
                           const QString &remark);

  void showCommandWidgetContextMenu(const QPoint &pos);

  void onCustomAction(const QListWidgetItem *item, const QString &message);

  void onSendCommandButtonClicked();

  void sendCommand(const QString &command);

  void updatePortMessageLog(const QByteArray &data, const QString &direction);

  void onScriptButtonClicked();

  void onTimeCheckBoxStateChanged(const qint16 &state);

  void uncheckTimeCheckBox();

  // 解码显示
  void onDecodeCheckBoxStateChanged(const qint16 &state);

  // 编码发送
  void onEncodeCheckBoxStateChanged(const qint16 &state);

  QByteArray appendCheckCode(const QByteArray &command,
                             const int &checkCodeType);

private slots:
  void executeTask(const QString &command);

  void onScriptFinished();
};

#endif // MAINWINDOW_H
