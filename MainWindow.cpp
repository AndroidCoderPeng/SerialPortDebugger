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
#include "Logger.hpp"
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
    : QMainWindow(parent), ui(new Ui::MainWindow), executorPtr(nullptr) {
  ui->setupUi(this);

  // 清除QComboBox的QAbstractItemView::item默认QSS
  ui->portNameBox->setView(new QListView());
  ui->baudRateBox->setView(new QListView());
  ui->dataBitBox->setView(new QListView());
  ui->parityBitBox->setView(new QListView());
  ui->stopBitBox->setView(new QListView());
  ui->checkCodeBox->setView(new QListView());

  initPortParam(ui);

  DatabaseWrapper::get()->init();
  updateCommandList();

  ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);

  QIntValidator *validator = new QIntValidator(1, 99999, this);
  ui->timeLineEdit->setValidator(validator);

  // 初始化校验码类型下拉框
  for (const QString &type : checkCodeTypes) {
    ui->checkCodeBox->addItem(type);
  }

  // 初始化发送/接收字节数
  updateTxRxBytes();

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
  connect(ui->listWidget, &QListWidget::itemClicked, this,
          &MainWindow::onCommandItemClicked);
  connect(ui->listWidget, &QListWidget::customContextMenuRequested, this,
          &MainWindow::showCommandWidgetContextMenu);
  connect(ui->sendDataButton, &QPushButton::clicked, this,
          &MainWindow::onSendCommandButtonClicked);
  connect(ui->historyListWidget, &QListWidget::itemClicked, this,
          &MainWindow::onHistoryItemClicked);
  connect(ui->scriptButton, &QPushButton::clicked, this,
          &MainWindow::onScriptButtonClicked);
  connect(ui->timeCheckBox, &QCheckBox::stateChanged, this,
          &MainWindow::onTimeCheckBoxStateChanged);
  connect(ui->hexReceiveCheckBox, &QCheckBox::stateChanged, this,
          &MainWindow::onDecodeCheckBoxStateChanged);
  connect(ui->hexSendCheckBox, &QCheckBox::stateChanged, this,
          &MainWindow::onEncodeCheckBoxStateChanged);

  // ========== 一次性连接 SerialPortManager 的所有信号 ==========
  SerialPortManager *mgr = SerialPortManager::get();
  connect(mgr, &SerialPortManager::dataReceivedSignal, this,
          [this](const QByteArray &data) { updatePortMessageLog(data, "收"); });
  connect(mgr, &SerialPortManager::stateChangedSignal, this,
          [this](bool opened) {
            ui->openPortButton->setText(opened ? "关闭串口" : "打开串口");
            updateComboxState(opened);
            if (!opened) {
              uncheckTimeCheckBox();
              if (timerPtr && timerPtr->isActive()) {
                timerPtr->stop();
              }
            }
          });
  connect(mgr, &SerialPortManager::errorOccurredSignal, this,
          [this](const QString &msg) {
            QMessageBox::critical(this, "串口错误", msg);
          });
}

void MainWindow::updateCommandList() {
  ui->listWidget->clear();
  const auto commands = DatabaseWrapper::get()->getAllCommands();
  for (int i = 0; i < commands.size(); ++i) {
    if (i == commands.size() - 1) {
      // 末尾固定显示"添加新指令"占位项
      updateCommandWidget(kAddItemMagicId, "＋", "添加新指令");
    } else {
      const auto cmd = commands.at(i);
      updateCommandWidget(cmd.id, cmd.value, cmd.remark);
    }
  }
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
  SerialPortManager *mgr = SerialPortManager::get();
  if (mgr->isOpen()) {
    mgr->close();
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

    if (!ok) {
      QMessageBox::warning(this, "错误", "波特率转换失败");
      return;
    }

    // 默认无流控
    mgr->open(portName, baudRate, dataBits, parity, stopBits, "None");
  }
}

void MainWindow::onRefreshButtonClicked() {
  ui->portNameBox->clear();
  const auto &ports = QSerialPortInfo::availablePorts();
  if (ports.isEmpty()) {
    ui->portNameBox->addItem("No port available");
    return;
  }

  for (const QSerialPortInfo &port : ports) {
    ui->portNameBox->addItem(port.portName());
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

void MainWindow::onClearDataButtonClicked() {
  ui->messageView->clear();
  history.clear();
  txBytes = 0;
  rxBytes = 0;
  updateTxRxBytes();
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
  // 如果点击的是"添加新指令"占位项 → 弹出添加对话框
  if (item->data(Qt::UserRole).toInt() == kAddItemMagicId) {
    onAddCommandItemClicked();
    return;
  }

  auto *listItem = const_cast<QListWidgetItem *>(item);
  auto *itemWidget =
      qobject_cast<CommandItemWidget *>(ui->listWidget->itemWidget(listItem));
  if (!itemWidget) {
    return;
  }

  ui->userInputView->setText(itemWidget->command());
}

void MainWindow::onAddCommandItemClicked() {
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
    updateCommandList();
  }
}

void MainWindow::showCommandWidgetContextMenu(const QPoint &pos) {
  const auto listWidget = qobject_cast<QListWidget *>(sender());
  if (listWidget) {
    const QListWidgetItem *item = listWidget->itemAt(pos);
    if (item == nullptr)
      return;

    // "添加新指令"占位项不显示右键菜单
    if (item->data(Qt::UserRole).toInt() == kAddItemMagicId) {
      return;
    }

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
  const auto command = ui->userInputView->toPlainText();
  sendCommand(command);
}

void MainWindow::sendCommand(const QString &command) {
  if (!SerialPortManager::get()->isOpen()) {
    QMessageBox::warning(this, "警告", "请先打开串口！");
    return;
  }

  QByteArray data{};
  if (ui->hexSendCheckBox->isChecked()) {
    // 检查输入的是不是16进制字符串
    if (!Utils::isHexString(command)) {
      QMessageBox::warning(this, "警告", "请输入16进制字符串！");
      return;
    }
    const QByteArray value = Utils::formatHexString(command);

    // 需要判断校验码方式
    const auto currentIndex = ui->checkCodeBox->currentIndex();
    Logger::Tag("MainWindow")
        .dFmt("采用【%s】校验方式发送指令",
              checkCodeTypes[currentIndex].toStdString().c_str());
    data = appendCheckCode(value, currentIndex);
  } else {
    data = command.toUtf8();
  }
  SerialPortManager::get()->write(data);
  updatePortMessageLog(data, "发");
  updateHistoryListWidget(command);
}

void MainWindow::updatePortMessageLog(const QByteArray &data,
                                      const QString &direction) {
  if (direction == "收") {
    rxBytes += data.size();
  } else {
    txBytes += data.size();
  }
  updateTxRxBytes();

  const PortMessage msg(data, direction, QDateTime::currentMSecsSinceEpoch());
  history.append(msg);

  QString dataStr;
  if (ui->hexReceiveCheckBox->isChecked()) {
    dataStr = Utils::formatByteArray(data);
  } else {
    dataStr = QString(data);
  }

  QTextCursor cursor(ui->messageView->document());
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

  ui->messageView->setTextCursor(cursor);
  ui->messageView->ensureCursorVisible(); // 自动滚到底部
}

void MainWindow::updateHistoryListWidget(const QString &command) {
  // 去重：如果已存在相同指令，先移除旧的
  for (int i = 0; i < ui->historyListWidget->count(); ++i) {
    if (ui->historyListWidget->item(i)->text() == command) {
      delete ui->historyListWidget->takeItem(i);
      break;
    }
  }

  // 插入到列表顶部
  ui->historyListWidget->insertItem(0, command);

  // 限制历史记录数量（保留最近50条）
  constexpr int maxHistory = 50;
  while (ui->historyListWidget->count() > maxHistory) {
    delete ui->historyListWidget->takeItem(ui->historyListWidget->count() - 1);
  }
}

void MainWindow::onHistoryItemClicked(QListWidgetItem *item) {
  ui->userInputView->setText(item->text());
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
        const auto command = ui->userInputView->toPlainText();
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

void MainWindow::onDecodeCheckBoxStateChanged(const qint16 &state) {
  ui->messageView->clear(); // 清空当前显示
  const QList<PortMessage> &listRef = history;
  if (state == Qt::Checked) {
    for (const auto &msg : listRef) {
      // 十六进制格式数据
      const QString hexData = Utils::formatByteArray(msg.data);

      QTextCursor cursor(ui->messageView->document());
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

      QTextCursor cursor(ui->messageView->document());
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

  ui->messageView->ensureCursorVisible(); // 自动滚到底部
}

void MainWindow::onEncodeCheckBoxStateChanged(const qint16 &state) {
  if (state == Qt::Checked) {
    ui->checkCodeBox->setDisabled(false);
  } else {
    ui->checkCodeBox->setDisabled(true);
  }
}

QByteArray MainWindow::appendCheckCode(const QByteArray &command,
                                       const int &checkCodeType) {
  switch (checkCodeType) {
  case 0:
    // 无校验码，直接返回原始指令
    return command;
  case 1:
    // CRC-8 校验码
    {
      uint8_t crc8 = Utils::calculateCRC8(command);
      QByteArray result = command;
      result.append(crc8);
      return result;
    }
    break;
  case 2:
    // CRC-16(L) 校验码（低字节在前）
    {
      uint16_t crc16 = Utils::calculateCRC16(command);
      QByteArray result = command;
      result.append(crc16 & 0xFF);        // 低字节
      result.append((crc16 >> 8) & 0xFF); // 高字节
      return result;
    }
    break;
  case 3:
    // CRC-16(H) 校验码（高字节在前）
    {
      uint16_t crc16 = Utils::calculateCRC16(command);
      QByteArray result = command;
      result.append((crc16 >> 8) & 0xFF); // 高字节
      result.append(crc16 & 0xFF);        // 低字节
      return result;
    }
    break;
  case 4:
    // XOR 校验码
    {
      uint8_t xorCode = Utils::calculateXOR(command);
      QByteArray result = command;
      result.append(xorCode);
      return result;
    }
    break;
  case 5:
    // Checksum 校验码（简单求和取反）
    {
      uint8_t checksum = Utils::calculateChecksum(command);
      QByteArray result = command;
      result.append(checksum);
      return result;
    }
    break;
  default:
    // 理论上不应该出现这个分支，因为 UI 上的下拉框选项是固定的
    // 为了安全起见，还是加上默认分支，直接返回原始指令
    return command;
  }
}

void MainWindow::updateTxRxBytes() {
  auto formatBytes = [](qint64 bytes) -> QString {
    if (bytes < 1024) {
      return QString("%1 B").arg(bytes);
    } else if (bytes < 1024 * 1024) {
      return QString("%1 KB").arg(bytes / 1024.0, 0, 'f', 1);
    } else {
      return QString("%1 MB").arg(bytes / (1024.0 * 1024.0), 0, 'f', 2);
    }
  };

  ui->rxLabel->setText(QString("已接收: %1").arg(formatBytes(rxBytes)));
  ui->txLabel->setText(QString("已发送: %1").arg(formatBytes(txBytes)));
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
