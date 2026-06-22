//
// Created by Administrator on 2025/5/2.
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QListWidgetItem>
#include <QMainWindow>

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

  TaskExecutor *executorPtr = nullptr;

  QList<PortMessage> history;
  QTimer *timerPtr = nullptr;
  QList<QString> checkCodeTypes = {"无校验码",  "CRC-8", "CRC-16(L)",
                                   "CRC-16(H)", "XOR",   "Checksum"};
  static constexpr int kAddItemMagicId = -999; // 标记"添加新指令"占位项
  qint64 txBytes = 0;                          // 发送字节数
  qint64 rxBytes = 0;                          // 接收字节数
  bool autoScrollEnabled = true;               // 是否自动滚动到底部
  bool hexReceiveEnabled = true; // 接收数据是否以十六进制显示

  // ====== 菜单栏 ======
  void onActionSaveDataClicked();

  void onActionClearDataClicked();

  void onActionDarkThemeToggled(bool checked);

  void onActionAutoScrollToggled(bool checked);

  void onActionProjectSiteTriggered();

  void onActionAboutTriggered();

  void onActionDecodeStateChanged(const int &state);

  // ==== 普通操作 ======
  void updateCommandList();

  void updateComboxState(bool disabled) const;

  void onOpenPortButtonClicked();

  void onRefreshButtonClicked();

  void onCommandItemClicked(const QListWidgetItem *item);

  void onAddCommandItemClicked();

  void updateCommandWidget(const qint16 &id, const QString &command,
                           const QString &remark);

  void showCommandWidgetContextMenu(const QPoint &pos);

  void onCustomAction(const QListWidgetItem *item, const QString &message);

  void onSendCommandButtonClicked();

  void sendCommand(const QString &command);

  void updatePortMessageLog(const QByteArray &data, const QString &direction);

  void updateHistoryListWidget(const QString &command);

  void onHistoryItemClicked(QListWidgetItem *item);

  void onScriptButtonClicked();

  void onTimeCheckBoxStateChanged(const qint16 &state);

  void uncheckTimeCheckBox();

  // 编码发送
  void onEncodeCheckBoxStateChanged(const qint16 &state);

  QByteArray appendCheckCode(const QByteArray &command,
                             const int &checkCodeType);

  void updateTxRxBytes();

private slots:
  void executeTask(const QString &command);

  void onScriptFinished();
};

#endif // MAINWINDOW_H
