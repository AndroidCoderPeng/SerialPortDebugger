//
// Created by Administrator on 2025/5/2.
//

#include "MainWindow.hpp"
#include "ui_MainWindow.h"

#include <QClipboard>
#include <QDateTime>
#include <QDebug>
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QTimer>

#include "CommandItemWidget.hpp"
#include "CommandScriptDialog.hpp"
#include "DatabaseWrapper.hpp"
#include "SaveCommandDialog.hpp"
#include "SerialPortManager.hpp"
#include "TaskExecutor.hpp"
#include "Utils.hpp"

static void initPortParam(const Ui::MainWindow *ui) {
  // 获取电脑串口
  const auto &ports = QSerialPortInfo::availablePorts();
  for (const QSerialPortInfo &port : ports) {
    ui->portNameBox->addItem(port.portName());
  }

  if (ui->portNameBox->count() == 0) {
    ui->portNameBox->addItem("No port available");
  }

  // 设置波特率
  const char *baudRates[] = {"9600",  "14400",  "19200",  "38400", "56000",
                             "57600", "115200", "128000", "230400"};
  for (const QString &rate : baudRates) {
    ui->baudRateBox->addItem(rate);
  }

  // 设置数据位
  const char *dataBits[] = {"5", "6", "7", "8"};

  for (const QString &bit : dataBits) {
    ui->dataBitBox->addItem(bit);
  }
  // 设置默认数据位为 8
  ui->dataBitBox->setCurrentText("8");

  // 设置奇偶校验
  const char *parityBits[] = {"None", "Even", "Odd", "Mark", "Space"};

  for (const QString &parity : parityBits) {
    ui->parityBitBox->addItem(parity);
  }

  // 设置停止位
  const char *stopBits[] = {"1", "1.5", "2"};

  for (const QString &stopBit : stopBits) {
    ui->stopBitBox->addItem(stopBit);
  }
}

MainWindow::MainWindow(QMainWindow *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), _logger("MainWindow"),
      executorPtr(nullptr) {
  ui->setupUi(this);

  // 清除QComboBox的QAbstractItemView::item默认QSS
  ui->portNameBox->setView(new QListView());
  ui->baudRateBox->setView(new QListView());
  ui->dataBitBox->setView(new QListView());
  ui->parityBitBox->setView(new QListView());
  ui->stopBitBox->setView(new QListView());

  initPortParam(ui);

  DatabaseWrapper::get()->init();
  const auto commands = DatabaseWrapper::get()->getAllCommands();
  for (const auto &cmd : commands) {
    updateCommandWidget(cmd.id, cmd.value, cmd.remark);
  }

  ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);

  QIntValidator *validator = new QIntValidator(1, 99999, this);
  ui->timeLineEdit->setValidator(validator);

  // 初始化任务执行器
  executorPtr = new TaskExecutor(this);
  connect(executorPtr, &TaskExecutor::taskExecuted, this,
          &MainWindow::executeTask);
  connect(executorPtr, &TaskExecutor::finished, this,
          &MainWindow::onScriptFinished);

  // 连接信号槽
  connect(ui->openPortButton, &QPushButton::clicked, this,
          &MainWindow::onOpenPortButtonClicked);
  connect(ui->refreshButton, &QPushButton::clicked, this,
          &MainWindow::onRefreshButtonClicked);
  connect(ui->saveDataButton, &QPushButton::clicked, this,
          &MainWindow::onSaveDataButtonClicked);
  connect(ui->clearDataButton, &QPushButton::clicked, this,
          &MainWindow::onClearDataButtonClicked);
  connect(ui->addCommandButton, &QPushButton::clicked, this,
          &MainWindow::onAddCommandButtonClicked);
  connect(ui->listWidget, &QListWidget::itemClicked, this,
          &MainWindow::onCommandItemClicked);
  connect(ui->listWidget, &QListWidget::customContextMenuRequested, this,
          &MainWindow::showCommandWidgetContextMenu);
  connect(ui->sendDataButton, &QPushButton::clicked, this,
          &MainWindow::onSendCommandButtonClicked);
  connect(ui->scriptButton, &QPushButton::clicked, this,
          &MainWindow::onScriptButtonClicked);
  connect(ui->timeCheckBox, &QCheckBox::stateChanged, this,
          &MainWindow::onTimeCheckBoxStateChanged);
  connect(ui->hexReceiveCheckBox, &QCheckBox::stateChanged, this,
          &MainWindow::onEncodeCheckBoxStateChanged);
}

void MainWindow::updateComboxState(const bool disabled) const {
  auto updateState = [=](QComboBox *comboBox) {
    comboBox->setDisabled(disabled);
  };

  updateState(ui->portNameBox);
  updateState(ui->baudRateBox);
  updateState(ui->dataBitBox);
  updateState(ui->parityBitBox);
  updateState(ui->stopBitBox);
}

void MainWindow::onOpenPortButtonClicked() {
  if (SerialPortManager::get()->isOpen()) {
    SerialPortManager::get()->close();
    ui->openPortButton->setText("打开串口");
    updateComboxState(false);
    uncheckTimeCheckBox();
    if (timerPtr && timerPtr->isActive()) {
      timerPtr->stop();
    }
  } else {
    const QString portName = ui->portNameBox->currentText();
    bool ok;
    const int baudRate = ui->baudRateBox->currentText().toInt(&ok);
    const QString dataBits = ui->dataBitBox->currentText();
    const QString parity = ui->parityBitBox->currentText();
    const QString stopBits = ui->stopBitBox->currentText();
    const QString flowControl = "None"; // 默认无流控

    if (!ok) {
      QMessageBox::warning(this, "错误", "波特率转换失败");
      return;
    }

    const auto ret = SerialPortManager::get()->open(
        portName, baudRate, dataBits, parity, stopBits, flowControl);
    if (ret) {
      ui->openPortButton->setText("关闭串口");
      updateComboxState(true);
    } else {
      QMessageBox::critical(this, "错误", "打开失败，请检查参数设置和串口连接");
    }
  }
}

void MainWindow::slotDataReceived(const QByteArray &data) {
  updateComMessageLog(data, "收");
}

void MainWindow::onRefreshButtonClicked() {
  ui->portNameBox->clear();
  const auto &ports = QSerialPortInfo::availablePorts();
  for (const QSerialPortInfo &port : ports) {
    ui->portNameBox->addItem(port.portName());
  }

  if (ui->portNameBox->count() == 0) {
    ui->portNameBox->addItem("No serial ports available");
  }
}

void MainWindow::onSaveDataButtonClicked() {
  if (history.isEmpty()) {
    QMessageBox::warning(this, "警告", "没有数据可以保存");
    return;
  }
  const QString filePath =
      QFileDialog::getSaveFileName(this, "保存日志", "", "文本文件 (*.txt)");
  if (filePath.isEmpty()) {
    QMessageBox::warning(this, "警告", "未选择保存文件");
    return;
  }
  QFile file(filePath);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text |
                 QIODevice::Truncate)) {
    QMessageBox::critical(this, "错误", "无法打开文件：" + file.errorString());
    return;
  }
  QTextStream out(&file);
  const QList<PortMessage> &listRef = history;
  for (const auto &msg : listRef) {
    const QString hexData = Utils::formatByteArray(msg.data);
    const auto line = QString("[%1]【%2】%3\n")
                          .arg(msg.formattedTime, msg.direction, hexData);
    out << line;
  }
  file.close();
}

void MainWindow::onClearDataButtonClicked() { ui->comMessageView->clear(); }

void MainWindow::onAddCommandButtonClicked() {
  SaveCommandDialog dialog(this);
  if (dialog.exec() == QDialog::Accepted) {
    const auto command = dialog.getInputValue();
    const auto &value = command.value;
    const auto &remark = command.remark;

    if (DatabaseWrapper::get()->commandExists(value)) {
      QMessageBox::warning(this, "警告", "该指令值已存在！");
      return;
    }

    // 插入到数据库
    DatabaseWrapper::get()->addCommand(value, remark);
    // 重新加载列表
    ui->listWidget->clear();
    const auto commands = DatabaseWrapper::get()->getAllCommands();
    for (const auto &cmd : commands) {
      updateCommandWidget(cmd.id, cmd.value, cmd.remark);
    }
  }
}

void MainWindow::updateCommandWidget(const qint16 &id, const QString &command,
                                     const QString &remark) {
  auto *listItem = new QListWidgetItem(ui->listWidget);
  listItem->setData(Qt::UserRole, id);
  listItem->setFlags(listItem->flags() & ~Qt::ItemIsEditable);

  auto *itemWidget = new CommandItemWidget(command, remark, ui->listWidget);
  listItem->setSizeHint(itemWidget->sizeHint());
  ui->listWidget->setItemWidget(listItem, itemWidget);
}

void MainWindow::onCommandItemClicked(const QListWidgetItem *item) {
  auto *listItem = const_cast<QListWidgetItem *>(item);
  auto *itemWidget =
      qobject_cast<CommandItemWidget *>(ui->listWidget->itemWidget(listItem));
  if (!itemWidget) {
    return;
  }

#ifdef Q_OS_LINUX
  // 取消之前的选中状态
  if (previousSelectedItemPtr && previousSelectedItemPtr != listItem) {
    auto *prevWidget = qobject_cast<CommandItemWidget *>(
        ui->listWidget->itemWidget(previousSelectedItemPtr));
    if (prevWidget) {
      prevWidget->setSelected(false);
    }
  }

  // 设置当前项为选中状态
  itemWidget->setSelected(true);
  previousSelectedItemPtr = listItem;
#endif

  ui->userInputView->setText(itemWidget->command());
}

void MainWindow::showCommandWidgetContextMenu(const QPoint &pos) {
  const auto listWidget = qobject_cast<QListWidget *>(sender());
  if (listWidget) {
    const QListWidgetItem *item = listWidget->itemAt(pos);
    if (item != nullptr) {
      QMenu menu(this);
      const QAction *sendAction = menu.addAction("发送");
      const QAction *copyAction = menu.addAction("复制");
      const QAction *editAction = menu.addAction("编辑");
      const QAction *deleteAction = menu.addAction("删除");
      const QAction *selectedAction =
          menu.exec(listWidget->viewport()->mapToGlobal(pos));
      if (selectedAction == sendAction) {
        onCustomAction(item, "0");
      } else if (selectedAction == copyAction) {
        onCustomAction(item, "1");
      } else if (selectedAction == editAction) {
        onCustomAction(item, "2");
      } else if (selectedAction == deleteAction) {
        onCustomAction(item, "3");
      }
    }
  }
}

void MainWindow::onCustomAction(const QListWidgetItem *item,
                                const QString &message) {
  auto *listItem = const_cast<QListWidgetItem *>(item);
  auto *itemWidget =
      qobject_cast<CommandItemWidget *>(ui->listWidget->itemWidget(listItem));
  if (!itemWidget) {
    return;
  }

  const int id = item->data(Qt::UserRole).toInt();
  const QString command = itemWidget->command();
  const QString remark = itemWidget->remark();

  if (message == "0") {
    sendCommand(command);
  } else if (message == "1") {
    QApplication::clipboard()->setText(command);
  } else if (message == "2") {
    SaveCommandDialog dialog(this, command, remark);
    if (dialog.exec() == QDialog::Accepted) {
      const auto newCommand = dialog.getInputValue();
      const auto &newValue = newCommand.value;
      const auto &newRemark = newCommand.remark;

      // 检查是否重复
      if (DatabaseWrapper::get()->commandExists(newValue) &&
          newValue != command) {
        QMessageBox::warning(this, "警告", "该指令值已存在！");
        return;
      }

      // 更新数据库
      DatabaseWrapper::get()->updateCommand(id, newValue, newRemark);

      // 更新表格显示
      itemWidget->setContent(newValue, newRemark);
    }
  } else if (message == "3") {
    DatabaseWrapper::get()->deleteCommand(id);
    delete ui->listWidget->takeItem(ui->listWidget->row(listItem));
  }
}

void MainWindow::onSendCommandButtonClicked() {
  // 获取纯文字内容
  const auto command = ui->userInputView->text();
  sendCommand(command);
}

void MainWindow::sendCommand(const QString &command) {
  if (!SerialPortManager::get()->isOpen()) {
    QMessageBox::warning(this, "警告", "请先打开串口！");
    return;
  }

  if (ui->hexSendCheckBox->isChecked()) {
    // 检查输入的是不是16进制字符串
    if (!Utils::isHexString(command)) {
      QMessageBox::warning(this, "警告", "请输入16进制字符串！");
      return;
    }
    const QByteArray data = Utils::formatHexString(command);
    SerialPortManager::get()->write(data);
    updateComMessageLog(data, "发");
  } else {
    const QByteArray data = command.toUtf8();
    SerialPortManager::get()->write(data);
    updateComMessageLog(data, "发");
  }
}

void MainWindow::updateComMessageLog(const QByteArray &data,
                                     const QString &direction) {
  const PortMessage msg(data, direction, QDateTime::currentMSecsSinceEpoch());
  history.append(msg);

  QString dataStr;
  if (ui->hexReceiveCheckBox->isChecked()) {
    dataStr = Utils::formatByteArray(data);
  } else {
    dataStr = QString(data);
  }

  QTextCursor cursor(ui->comMessageView->document());
  cursor.movePosition(QTextCursor::End);
  if (msg.direction == "收") {
    QTextCharFormat format;
    format.setForeground(Qt::darkGreen); // 接收用绿色
    cursor.setCharFormat(format);
  } else {
    cursor.setCharFormat(QTextCharFormat()); // 恢复默认格式
  }
  cursor.insertText(
      QString("[%1]【%2】%3\n").arg(msg.formattedTime, msg.direction, dataStr));

  ui->comMessageView->setTextCursor(cursor);
  ui->comMessageView->ensureCursorVisible(); // 自动滚到底部
}

void MainWindow::onScriptButtonClicked() {
  if (!SerialPortManager::get()->isOpen()) {
    QMessageBox::warning(this, "警告", "请先打开串口！");
    return;
  }

  if (executorPtr && executorPtr->isRunning()) {
    QMessageBox::information(this, "提示",
                             "脚本正在执行，请先等待当前脚本完成");
    return;
  }

  const auto commands = DatabaseWrapper::get()->getAllCommands();

  if (commands.count() <= 1) {
    QMessageBox::information(this, "提示", "指令数量至少大于2");
    return;
  }

  CommandScriptDialog dialog(this, commands);
  if (dialog.exec() == QDialog::Accepted) {
    // 获取脚本参数，然后按照脚本执行命令
    const auto configs = dialog.getScriptConfigs();
    QList<Task> tasks;
    for (const ScriptConfig &config : configs) {
      Task task;
      task.command = config.command;
      task.interval = config.interval;
      tasks.append(task);
    }
    executorPtr->setTasks(tasks);
    executorPtr->start();
  }
}

void MainWindow::onTimeCheckBoxStateChanged(const qint16 &state) {
  if (!SerialPortManager::get()->isOpen()) {
    QMessageBox::warning(this, "警告", "请先打开串口！");
    uncheckTimeCheckBox();
    return;
  }

  if (state == Qt::Checked) {
    const QString time = ui->timeLineEdit->text();
    if (!Utils::isPositiveInt(time)) {
      QMessageBox::warning(this, "警告", "请输入正整数！");
      return;
    }

    if (!timerPtr) {
      timerPtr = new QTimer(this);
      connect(timerPtr, &QTimer::timeout, this, [this] {
        const auto command = ui->userInputView->text();
        sendCommand(command);
      });
    }
    timerPtr->start(time.toInt());
  } else if (state == Qt::Unchecked) {
    if (timerPtr && timerPtr->isActive()) {
      timerPtr->stop();
    }
  } else {
    qDebug() << "无效的状态值：" << state;
  }
}

void MainWindow::uncheckTimeCheckBox() {
  ui->timeCheckBox->blockSignals(true);
  ui->timeCheckBox->setCheckState(Qt::Unchecked);
  ui->timeCheckBox->blockSignals(false);
}

void MainWindow::onEncodeCheckBoxStateChanged(const qint16 &state) {
  ui->comMessageView->clear(); // 清空当前显示
  const QList<PortMessage> &listRef = history;
  if (state == Qt::Checked) {
    for (const auto &msg : listRef) {
      // 十六进制格式数据
      const QString hexData = Utils::formatByteArray(msg.data);

      QTextCursor cursor(ui->comMessageView->document());
      cursor.movePosition(QTextCursor::End);

      if (msg.direction == "收") {
        QTextCharFormat format;
        format.setForeground(Qt::darkGreen); // 接收用绿色
        cursor.setCharFormat(format);
      } else {
        cursor.setCharFormat(QTextCharFormat()); // 恢复默认格式
      }
      cursor.insertText(QString("[%1]【%2】%3\n")
                            .arg(msg.formattedTime, msg.direction, hexData));
    }
  } else {
    for (const auto &msg : listRef) {
      const QString decodedData = QString(msg.data);

      QTextCursor cursor(ui->comMessageView->document());
      cursor.movePosition(QTextCursor::End);

      if (msg.direction == "收") {
        QTextCharFormat format;
        format.setForeground(Qt::darkGreen); // 接收用绿色
        cursor.setCharFormat(format);
      } else {
        cursor.setCharFormat(QTextCharFormat()); // 恢复默认格式
      }
      cursor.insertText(
          QString("[%1]【%2】%3\n")
              .arg(msg.formattedTime, msg.direction, decodedData));
    }
  }

  ui->comMessageView->ensureCursorVisible(); // 自动滚到底部
}

void MainWindow::executeTask(const QString &command) { sendCommand(command); }

void MainWindow::onScriptFinished() {
  QMessageBox::information(this, "提示", "脚本执行完成");
}

MainWindow::~MainWindow() {
  SerialPortManager::get()->close();
  executorPtr->stop();
  uncheckTimeCheckBox();
  delete ui;
}
