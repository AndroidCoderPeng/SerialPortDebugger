#include "SerialPortManager.hpp"

#include <QMetaObject>
#include <QSerialPort>
#include <QThread>

class SerialPortWorker : public QObject {
  Q_OBJECT
public:
  explicit SerialPortWorker(QObject *parent = nullptr)
      : QObject(parent), portPtr(nullptr) {}

public slots:
  void init() {
    if (portPtr) {
      return;
    }

    portPtr = new QSerialPort(this);

    connect(portPtr, &QSerialPort::readyRead, this, [this]() {
      if (!portPtr || !portPtr->isOpen()) {
        return;
      }

      QByteArray data = portPtr->readAll();
      while (portPtr->waitForReadyRead(10)) {
        data += portPtr->readAll();
      }

      if (!data.isEmpty()) {
        emit dataReceived(data);
      }
    });

    connect(portPtr, &QSerialPort::errorOccurred, this,
            [this](QSerialPort::SerialPortError error) {
              if (error == QSerialPort::NoError ||
                  error == QSerialPort::TimeoutError) {
                return;
              }
              emit errorOccurred(portPtr ? portPtr->errorString()
                                         : QString("Unknown serial error"));
            });
  }

  bool openPort(const QString &portName, qint32 baudRate,
                const QString &dataBits, const QString &parity,
                const QString &stopBits, const QString &flowControl) {
    if (!portPtr) {
      emit errorOccurred("SerialPortWorker is not initialized");
      return false;
    }

    if (portPtr->isOpen()) {
      portPtr->close();
    }

    portPtr->setPortName(portName);
    portPtr->setBaudRate(baudRate);

    if (dataBits == "5") {
      portPtr->setDataBits(QSerialPort::Data5);
    } else if (dataBits == "6") {
      portPtr->setDataBits(QSerialPort::Data6);
    } else if (dataBits == "7") {
      portPtr->setDataBits(QSerialPort::Data7);
    } else {
      portPtr->setDataBits(QSerialPort::Data8);
    }

    if (parity == "Even") {
      portPtr->setParity(QSerialPort::EvenParity);
    } else if (parity == "Odd") {
      portPtr->setParity(QSerialPort::OddParity);
    } else {
      portPtr->setParity(QSerialPort::NoParity);
    }

    if (stopBits == "2") {
      portPtr->setStopBits(QSerialPort::TwoStop);
    } else {
      portPtr->setStopBits(QSerialPort::OneStop);
    }

    if (flowControl == "Hardware") {
      portPtr->setFlowControl(QSerialPort::HardwareControl);
    } else if (flowControl == "Software") {
      portPtr->setFlowControl(QSerialPort::SoftwareControl);
    } else {
      portPtr->setFlowControl(QSerialPort::NoFlowControl);
    }

    const bool ok = portPtr->open(QIODevice::ReadWrite);
    if (!ok) {
      emit errorOccurred(portPtr->errorString());
    }
    emit stateChanged(ok);
    return ok;
  }

  void writeData(const QByteArray &data) {
    if (!portPtr || !portPtr->isOpen()) {
      emit errorOccurred("Serial port is not open");
      return;
    }

    if (data.isEmpty()) {
      return;
    }

    const qint64 written = portPtr->write(data);
    if (written < 0) {
      emit errorOccurred(portPtr->errorString());
      return;
    }

    if (!portPtr->waitForBytesWritten(300)) {
      emit errorOccurred(portPtr->errorString());
    }
  }

  void closePort() {
    if (portPtr && portPtr->isOpen()) {
      portPtr->close();
    }
    emit stateChanged(false);
  }

  bool isPortOpen() const { return portPtr && portPtr->isOpen(); }

signals:
  void dataReceived(const QByteArray &data);
  void stateChanged(bool opened);
  void errorOccurred(const QString &message);

private:
  QSerialPort *portPtr;
};

SerialPortManager::SerialPortManager(QObject *parent)
    : QObject(parent), ioThreadPtr(new QThread(this)),
      workerPtr(new SerialPortWorker) {
  workerPtr->moveToThread(ioThreadPtr);

  connect(ioThreadPtr, &QThread::finished, workerPtr, &QObject::deleteLater);
  connect(workerPtr, &SerialPortWorker::dataReceived, this,
          &SerialPortManager::dataReceivedSlot);
  connect(workerPtr, &SerialPortWorker::stateChanged, this,
          &SerialPortManager::stateChangedSlot);
  connect(workerPtr, &SerialPortWorker::errorOccurred, this,
          &SerialPortManager::errorOccurredSlot);

  ioThreadPtr->start();

  // 确保在工作线程中初始化串口对象，避免主线程访问未初始化的对象
  QMetaObject::invokeMethod(workerPtr, "init", Qt::BlockingQueuedConnection);
}

bool SerialPortManager::open(const QString &portName, qint32 baudRate,
                             const QString &dataBits, const QString &parity,
                             const QString &stopBits,
                             const QString &flowControl) {
  bool ok = false;
  QMetaObject::invokeMethod(
      workerPtr,
      [&]() {
        ok = workerPtr->openPort(portName, baudRate, dataBits, parity, stopBits,
                                 flowControl);
      },
      Qt::BlockingQueuedConnection);
  return ok;
}

void SerialPortManager::write(const QByteArray &data) {
  QMetaObject::invokeMethod(
      workerPtr, [=]() { workerPtr->writeData(data); }, Qt::QueuedConnection);
}

void SerialPortManager::close() {
  QMetaObject::invokeMethod(workerPtr, "closePort",
                            Qt::BlockingQueuedConnection);
}

bool SerialPortManager::isOpen() const {
  bool opened = false;
  QMetaObject::invokeMethod(
      workerPtr, [&]() { opened = workerPtr->isPortOpen(); },
      Qt::BlockingQueuedConnection);
  return opened;
}

SerialPortManager::~SerialPortManager() {
  close();

  if (ioThreadPtr->isRunning()) {
    ioThreadPtr->quit();
    ioThreadPtr->wait();
  }
}

void SerialPortManager::dataReceivedSlot(const QByteArray &data) {
  emit dataReceivedSignal(data);
}

void SerialPortManager::stateChangedSlot(bool opened) {
  emit stateChangedSignal(opened);
}

void SerialPortManager::errorOccurredSlot(const QString &message) {
  emit errorOccurredSignal(message);
}

// 内部 Worker 类写在 .cpp，就需要这一行代码。
// 如果把内部 Worker 类挪到独立头文件并加入工程头文件列表，通常就不需要这行了。
#include "SerialPortManager.moc"