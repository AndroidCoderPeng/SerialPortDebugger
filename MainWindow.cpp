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
#include <QSerialPortInfo>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QTimer>

#include "combo_box_item_delegate.hpp"
#include "commandscriptdialog.hpp"
#include "savecommanddialog.hpp"
#include "taskexecutor.hpp"
#include "utils.hpp"

static void setComboxBoxStyle(const Ui::MainWindow *ui) {
  const auto materialComboBoxStyle = R"(
        QComboBox {
            background-color: #FFFFFF;
            color: #212121;
            border-radius: 4px;
            padding: 4px 6px;
            border: 1px solid #BDBDBD;
            selection-background-color: #6200EE;
            font: 9pt "微软雅黑";
        }

        QComboBox::down-arrow {
            image: url(:/ic_down_arrow.png);
            width: 16px;
            height: 16px;
        }

        QComboBox::drop-down {
            subcontrol-origin: padding;
            subcontrol-position: top right;
            width: 30px;
            border-left: 1px solid #BDBDBD;
            background-color: transparent;
        }

        QComboBox:hover {
            border-color: #757575;
        }

        QComboBox:pressed {
            background-color: #EEEEEE;
        }

        QComboBox:disabled {
            background-color: #E0E0E0;
            color: #9E9E9E;
            border-color: #BDBDBD;
        }

        QComboBox QAbstractItemView {
            outline: 0px;
            background-color: white;
            color: #212121;
            border: 1px solid #BDBDBD;
            selection-background-color: #E0E0E0;
            selection-color: #212121;
        }
    )";

  auto applyStyle = [=](QComboBox *comboBox) {
    comboBox->setStyleSheet(materialComboBoxStyle);
    comboBox->setItemDelegate(new ComboBoxItemDelegate(comboBox));
  };

  applyStyle(ui->portNameBox);
  applyStyle(ui->baudRateBox);
  applyStyle(ui->dataBitBox);
  applyStyle(ui->parityBitBox);
  applyStyle(ui->stopBitBox);
}

static void setCheckBoxStyle(const Ui::MainWindow *ui) {
  const auto materialCheckBoxStyle = R"(
        QCheckBox {
            spacing: 5px;
            color: #212121;
            font: 10pt "微软雅黑";
        }

        QCheckBox::indicator {
            width: 12px;
            height: 12px;
            background-color: #FFFFFF;
            border: 1px solid #9E9E9E;
            border-radius: 4px;
            padding: 2px;
        }

        QCheckBox::indicator:checked {
            background-color: #6200EE;
            border: 1px solid #6200EE;
            image: url(:/ic_check.png);
        }

        QCheckBox::indicator:hover {
            border: 1px solid #757575;
        }

        QCheckBox::indicator:disabled {
            background-color: #E0E0E0;
            border: 1px solid #BDBDBD;
        }
    )";

  auto applyStyle = [=](QCheckBox *checkBox) {
    checkBox->setStyleSheet(materialCheckBoxStyle);
  };

  applyStyle(ui->timeCheckBox);
  applyStyle(ui->hexReceiveCheckBox);
  applyStyle(ui->hexSendCheckBox);
}

static void setPlainTextEditStyle(const Ui::MainWindow *ui) {
  const auto materialPlainTextEditStyle = R"(
        QPlainTextEdit {
            background-color: #FFFFFF;
            color: #212121;
            border-radius: 4px;
            padding: 0px 2px;
            border: 1px solid #BDBDBD;
            font: 10pt "微软雅黑"
        }

        QPlainTextEdit:focus {
            border: 1px solid #6200EE
        }

        QPlainTextEdit:hover {
            border-color: #757575
        }

        QPlainTextEdit:disabled {
            background-color: #E0E0E0;
            color: #9E9E9E;
            border-color: #BDBDBD
        }

        QScrollBar:vertical {
            background-color: #F5F5F5;
            width: 12px;
            margin: 0px 0px 0px 0px
        }

        QScrollBar::handle:vertical {
            background-color: #BDBDBD;
            min-height: 20px;
            border-radius: 6px
        }

        QScrollBar::add-line:vertical,
        QScrollBar::sub-line:vertical {
            background-color: transparent
        }

        QScrollBar::add-page:vertical,
        QScrollBar::sub-page:vertical {
            background-color: transparent
        }
    )";

  auto applyStyle = [=](QPlainTextEdit *textEdit) {
    textEdit->setStyleSheet(materialPlainTextEditStyle);
  };

  applyStyle(ui->comMessageView);
  applyStyle(ui->userInputView);
}

static void initParam(const Ui::MainWindow *ui) {
  // 获取电脑串口
  const auto &ports = QSerialPortInfo::availablePorts();
  for (const QSerialPortInfo &port : ports) {
    ui->portNameBox->addItem(port.portName());
  }

  if (ui->portNameBox->count() == 0) {
    ui->portNameBox->addItem("No serial ports available");
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
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  setComboxBoxStyle(ui);
  setCheckBoxStyle(ui);
  setPlainTextEditStyle(ui);

  initParam(ui);

  initDatabase();

  const QStringList headerLabels = {"指令值", "备注"};
  ui->tableWidget->setHorizontalHeaderLabels(headerLabels);
  // 序号文字居中
  ui->tableWidget->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
  // ui渲染完之后获取tableWidget真实宽度
  QTimer::singleShot(0, this, [this] {
    int indexColumnWidth = ui->tableWidget->verticalHeader()->width();
    const int availableWidth = ui->tableWidget->width() - indexColumnWidth;
    ui->tableWidget->setColumnWidth(0, static_cast<int>(availableWidth * 0.7));
    ui->tableWidget->setColumnWidth(1, static_cast<int>(availableWidth * 0.3));
  });
  for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
    QTableWidgetItem *item = ui->tableWidget->item(row, 1);
    if (item) {
      item->setTextAlignment(Qt::AlignCenter);
    }
  }
  ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);

  QIntValidator *validator = new QIntValidator(1, 99999);
  ui->timeLineEdit->setValidator(validator);

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
  connect(ui->tableWidget, &QTableWidget::itemClicked, this,
          &MainWindow::onTableItemClicked);
  connect(ui->tableWidget, &QTableWidget::customContextMenuRequested, this,
          &MainWindow::showTableWidgetContextMenu);
  connect(ui->sendDataButton, &QPushButton::clicked, this,
          &MainWindow::onSendCommandButtonClicked);
  connect(ui->scriptButton, &QPushButton::clicked, this,
          &MainWindow::onScriptButtonClicked);
  connect(ui->timeCheckBox, &QCheckBox::stateChanged, this,
          &MainWindow::onTimeCheckBoxStateChanged);
  connect(ui->hexReceiveCheckBox, &QCheckBox::stateChanged, this,
          &MainWindow::onEncodeCheckBoxStateChanged);
}

void MainWindow::initDatabase() {
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName("commands.db");
  if (!db.open()) {
    qDebug() << db.lastError().text();
    return;
  }
  sqlQuery = new QSqlQuery(db);
  sqlQuery->exec("CREATE TABLE IF NOT EXISTS commands (id INTEGER PRIMARY KEY, "
                 "command TEXT, remark TEXT)");

  // 添加几条默认指令（上电、下电、电位查询）
  sqlQuery->exec("SELECT COUNT(*) FROM commands");
  sqlQuery->next();
  int count = sqlQuery->value(0).toInt();
  if (count == 0) {
    // 插入默认指令
    struct DefaultCommand {
      QString command;
      QString remark;
    };

    QList<DefaultCommand> defaults = {
        {"FE FE 02 10 FA", "上电"},
        {"FE FE 02 13 FA", "下电"},
        {"FE FE 02 3D FA", "电位查询"},
        {"FE FE 0F 3C 08 00 08 00 08 00 08 00 08 00 08 00 14 FA", "复位"},
        {"FE FE 03 12 01 FA", "状态检测"}};

    sqlQuery->prepare("INSERT INTO commands (command, remark) VALUES (?, ?)");
    for (const auto &cmd : defaults) {
      sqlQuery->addBindValue(cmd.command);
      sqlQuery->addBindValue(cmd.remark);
      if (!sqlQuery->exec()) {
        qDebug() << "插入默认指令失败：" << sqlQuery->lastError().text();
      }
    }
  }

  if (!sqlQuery->exec("SELECT id, command, remark FROM commands")) {
    qDebug() << sqlQuery->lastError().text();
    return;
  }

  while (sqlQuery->next()) {
    // 获取主键 ID
    int id = sqlQuery->value(0).toInt();
    QString command = sqlQuery->value(1).toString();
    QString remark = sqlQuery->value(2).toString();
    updateCommandTableWidget(id, command, remark);
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
  if (serialPort.isOpen()) {
    serialPort.close();
    ui->openPortButton->setText("打开串口");
    updateComboxState(false);
    updateConnectState(false);
    uncheckTimeCheckBox();
    if (timer && timer->isActive()) {
      timer->stop();
    }
  } else {
    serialPort.setPortName(ui->portNameBox->currentText());
    bool ok;
    const int baudRate = ui->baudRateBox->currentText().toInt(&ok);
    if (ok) {
      serialPort.setBaudRate(baudRate);
    } else {
      qDebug() << "波特率转换失败，使用默认值 9600";
      serialPort.setBaudRate(9600);
    }

    const QString dataBitsStr = ui->dataBitBox->currentText();
    QSerialPort::DataBits dataBits;
    if (dataBitsStr == "5") {
      dataBits = QSerialPort::Data5;
    } else if (dataBitsStr == "6") {
      dataBits = QSerialPort::Data6;
    } else if (dataBitsStr == "7") {
      dataBits = QSerialPort::Data7;
    } else if (dataBitsStr == "8") {
      dataBits = QSerialPort::Data8;
    } else {
      dataBits = QSerialPort::UnknownDataBits;
    }
    serialPort.setDataBits(dataBits);

    const QString parityStr = ui->parityBitBox->currentText();
    QSerialPort::Parity parity;
    if (parityStr == "None") {
      parity = QSerialPort::NoParity;
    } else if (parityStr == "Even") {
      parity = QSerialPort::EvenParity;
    } else if (parityStr == "Odd") {
      parity = QSerialPort::OddParity;
    } else if (parityStr == "Mark") {
      parity = QSerialPort::MarkParity;
    } else if (parityStr == "Space") {
      parity = QSerialPort::SpaceParity;
    } else {
      parity = QSerialPort::UnknownParity;
    }
    serialPort.setParity(parity);

    const QString stopBitsStr = ui->stopBitBox->currentText();
    QSerialPort::StopBits stopBits;
    if (stopBitsStr == "1") {
      stopBits = QSerialPort::OneStop;
    } else if (stopBitsStr == "1.5") {
      stopBits = QSerialPort::OneAndHalfStop;
    } else if (stopBitsStr == "2") {
      stopBits = QSerialPort::TwoStop;
    } else {
      stopBits = QSerialPort::UnknownStopBits;
    }
    serialPort.setStopBits(stopBits);

    // 以读写模式打开串口
    if (serialPort.open(QIODevice::ReadWrite)) {
      ui->openPortButton->setText("关闭串口");
      updateComboxState(true);
      updateConnectState(true);
      connect(&serialPort, &QSerialPort::readyRead, this,
              &MainWindow::onReceivedData);
    } else {
      QMessageBox::critical(this, "错误", serialPort.errorString());
    }
  }
}

void MainWindow::updateConnectState(const bool connected) const {
  QString materialLabelStyle;
  if (connected) {
    // 已连接
    materialLabelStyle = R"(
            QLabel {
                background-color: #0EB83A;
                border-radius: 8px;
            }
        )";
  } else {
    // 已断开
    materialLabelStyle = R"(
            QLabel {
                background-color: #EF5350;
                border-radius: 8px;
            }
        )";
  }
  ui->stateView->setStyleSheet(materialLabelStyle);
}

void MainWindow::onReceivedData() {
  bufferReceived += serialPort.readAll();
  while (true) {
    int endIndex = bufferReceived.indexOf('\n');
    if (endIndex == -1)
      break;

    updateComMessageLog(bufferReceived, "收");
    bufferReceived.remove(0, endIndex + 1);
  }
}

void MainWindow::slotDataReceived(const QByteArray &data) {
  // updateComMessageLog(data, "收");
  qDebug() << "Received data from SerialPortObserver: " << data;
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
  const QList<ComMessage> &listRef = history;
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
    const auto &value = command.getValue();
    const auto &remark = command.getRemark();

    // 检查是否已存在该指令值
    sqlQuery->prepare("SELECT COUNT(*) FROM commands WHERE command = ?");
    sqlQuery->addBindValue(value);
    if (!sqlQuery->exec()) {
      qDebug() << "查询失败：" << sqlQuery->lastError().text();
      return;
    }

    if (sqlQuery->next() && sqlQuery->value(0).toInt() > 0) {
      QMessageBox::warning(this, "警告", "该指令值已存在！");
      return;
    }

    // 插入到数据库
    sqlQuery->prepare("INSERT INTO commands (command, remark) VALUES (?, ?)");
    sqlQuery->addBindValue(value);
    sqlQuery->addBindValue(remark);
    if (!sqlQuery->exec()) {
      qDebug() << "插入失败：" << sqlQuery->lastError().text();
    } else {
      // 获取插入记录的主键 ID
      int insertedId = sqlQuery->lastInsertId().toInt();
      updateCommandTableWidget(insertedId, value, remark);
    }
  }
}

void MainWindow::updateCommandTableWidget(const qint16 &id,
                                          const QString &command,
                                          const QString &remark) {
  const int row = ui->tableWidget->rowCount();
  ui->tableWidget->insertRow(row);

  commandItem = new QTableWidgetItem(command);
  ui->tableWidget->setItem(row, 0, commandItem);

  remarkItem = new QTableWidgetItem(remark);
  remarkItem->setTextAlignment(Qt::AlignCenter);
  ui->tableWidget->setItem(row, 1, remarkItem);

  // 禁用双击显示编辑框
  commandItem->setFlags(commandItem->flags() & ~Qt::ItemIsEditable);
  remarkItem->setFlags(remarkItem->flags() & ~Qt::ItemIsEditable);

  // item绑定数据库主键ID
  commandItem->setData(Qt::UserRole, id);
  remarkItem->setData(Qt::UserRole, id);
}

void MainWindow::onTableItemClicked(const QTableWidgetItem *item) {
  const int row = item->row();
  const QString command = ui->tableWidget->item(row, 0)->text();
  ui->userInputView->setPlainText(command);
}

void MainWindow::showTableWidgetContextMenu(const QPoint &pos) {
  const auto tableWidget = qobject_cast<QTableWidget *>(sender());
  if (tableWidget) {
    const QTableWidgetItem *item = tableWidget->itemAt(pos);
    if (item != nullptr) {
      QMenu menu(this);
      const auto materialQMenuStyle = R"(
            QMenu {
                font: 10pt '微软雅黑';
            }
        )";
      menu.setStyleSheet(materialQMenuStyle);
      const QAction *sendAction = menu.addAction("发送");
      const QAction *copyAction = menu.addAction("复制");
      const QAction *editAction = menu.addAction("编辑");
      const QAction *deleteAction = menu.addAction("删除");
      const QAction *selectedAction =
          menu.exec(tableWidget->viewport()->mapToGlobal(pos));
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

void MainWindow::onCustomAction(const QTableWidgetItem *item,
                                const QString &message) {
  const int id = item->data(Qt::UserRole).value<qint16>();
  const int row = item->row();
  const QString command = ui->tableWidget->item(row, 0)->text();
  if (message == "0") {
    sendCommand(command);
  } else if (message == "1") {
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(command);
  } else if (message == "2") {
    const QString remark = ui->tableWidget->item(row, 1)->text();
    SaveCommandDialog dialog(this, command, remark);
    if (dialog.exec() == QDialog::Accepted) {
      const auto newCommand = dialog.getInputValue();
      const auto &newValue = newCommand.getValue();
      const auto &newRemark = newCommand.getRemark();

      // 检查是否重复
      sqlQuery->prepare("SELECT COUNT(*) FROM commands WHERE command = ?");
      sqlQuery->addBindValue(newValue);
      if (!sqlQuery->exec()) {
        qDebug() << "查询失败：" << sqlQuery->lastError().text();
        return;
      }

      // 更新数据库
      sqlQuery->prepare(
          "UPDATE commands SET command = ?, remark = ? WHERE id = ?");
      sqlQuery->addBindValue(newValue);
      sqlQuery->addBindValue(newRemark);
      sqlQuery->addBindValue(id);
      if (!sqlQuery->exec()) {
        qDebug() << "更新失败：" << sqlQuery->lastError().text();
      } else {
        // 更新表格显示
        QTableWidgetItem *cmdItem = ui->tableWidget->item(row, 0);
        cmdItem->setText(newValue);

        QTableWidgetItem *rmkItem = ui->tableWidget->item(row, 1);
        rmkItem->setText(newRemark);
      }
    }
  } else if (message == "3") {
    sqlQuery->prepare("DELETE FROM commands WHERE id = ?");
    sqlQuery->addBindValue(id);
    sqlQuery->exec();
    ui->tableWidget->removeRow(item->row());
  }
}

void MainWindow::onSendCommandButtonClicked() {
  // 获取纯文字内容
  const auto command = ui->userInputView->toPlainText();
  sendCommand(command);
}

void MainWindow::sendCommand(const QString &command) {
  if (!serialPort.isOpen()) {
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
    serialPort.write(data);
    updateComMessageLog(data, "发");
  } else {
    const QByteArray data = command.toUtf8();
    serialPort.write(data);
    updateComMessageLog(data, "发");
  }
}

void MainWindow::updateComMessageLog(const QByteArray &data,
                                     const QString &direction) {
  const ComMessage msg(data, direction, QDateTime::currentMSecsSinceEpoch());
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
  if (!serialPort.isOpen()) {
    QMessageBox::warning(this, "警告", "请先打开串口！");
    return;
  }

  if (!sqlQuery->exec("SELECT id, command, remark FROM commands")) {
    qDebug() << sqlQuery->lastError().text();
    return;
  }

  QList<Command> commands;
  while (sqlQuery->next()) {
    Command cmd;
    cmd.setId(sqlQuery->value(0).toInt());
    cmd.setValue(sqlQuery->value(1).toString());
    cmd.setRemark(sqlQuery->value(2).toString());
    commands.append(cmd);
  }

  if (commands.count() <= 1) {
    QMessageBox::information(this, "提示", "指令数量至少大于2");
    return;
  }

  CommandScriptDialog dialog(this, commands);
  if (dialog.exec() == QDialog::Accepted) {
    // 获取脚本参数，然后按照脚本执行命令
    const auto configs = dialog.getScriptConfigs();
    TaskExecutor *executor = new TaskExecutor(this);
    for (const ScriptConfig &config : configs) {
      executor->addTask(config.getCommand(), config.getInterval());
    }
    connect(executor, &TaskExecutor::taskExecuted, this,
            [this](const QString &command) { sendCommand(command); });
    executor->start();
  }
}

void MainWindow::onTimeCheckBoxStateChanged(const qint16 &state) {
  if (!serialPort.isOpen()) {
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

    if (!timer) {
      timer = new QTimer(this);
      connect(timer, &QTimer::timeout, this, [this] {
        const auto command = ui->userInputView->toPlainText();
        sendCommand(command);
      });
    }
    timer->start(time.toInt());
  } else if (state == Qt::Unchecked) {
    if (timer && timer->isActive()) {
      timer->stop();
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
  const QList<ComMessage> &listRef = history;
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

MainWindow::~MainWindow() {
  if (serialPort.isOpen()) {
    serialPort.close();
    uncheckTimeCheckBox();
  }
  delete ui;
}
