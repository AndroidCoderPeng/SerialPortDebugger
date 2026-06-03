//
// Created by Administrator on 2025/5/2.
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlQuery>
#include <QTableWidgetItem>

#include "Logger.hpp"
#include "com_message.hpp"

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
  QSqlQuery *sqlQuery;
  QTableWidgetItem *commandItem = nullptr;
  QTableWidgetItem *remarkItem = nullptr;
  QByteArray bufferReceived;
  QList<ComMessage> history;
  QTimer *timer = nullptr;

  void initDatabase();

  void updateComboxState(bool disabled) const;

  void onOpenPortButtonClicked();

  void updateConnectState(bool connected) const;

  void onRefreshButtonClicked();

  void onSaveDataButtonClicked();

  void onClearDataButtonClicked();

  void onAddCommandButtonClicked();

  void onTableItemClicked(const QTableWidgetItem *item);

  void updateCommandTableWidget(const qint16 &id, const QString &command,
                                const QString &remark);

  void showTableWidgetContextMenu(const QPoint &pos);

  void onCustomAction(const QTableWidgetItem *item, const QString &message);

  void onSendCommandButtonClicked();

  void sendCommand(const QString &command);

  void updateComMessageLog(const QByteArray &data, const QString &direction);

  void onScriptButtonClicked();

  void onTimeCheckBoxStateChanged(const qint16 &state);

  void uncheckTimeCheckBox();

  void onEncodeCheckBoxStateChanged(const qint16 &state);
};

#endif // MAINWINDOW_H
