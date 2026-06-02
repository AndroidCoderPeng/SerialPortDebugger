#include "SerialPortObserver.hpp"

#ifdef _WIN32
#define DefaultPort "COM1"
#else
#define DefaultPort "/dev/ttyS0"
#endif
#define DefaultBaudRate 19200

std::unique_ptr<SerialPortObserver> SerialPortObserver::_instance = nullptr;

SerialPortObserver::SerialPortObserver()
    : _logger("SerialPortObserver"), _portName(DefaultPort),
      _baudRate(DefaultBaudRate) {
  _logger.i("initialized succeed.");
}

SerialPortObserver *SerialPortObserver::get() {
  if (!_instance) {
    _instance = std::unique_ptr<SerialPortObserver>(new SerialPortObserver());
    _instance->start();
  }
  return _instance.get();
}

void SerialPortObserver::open(const QString &portName, qint32 baudRate) {
  _portName = portName;
  _baudRate = baudRate;

  if (portPtr && portPtr->isOpen()) {
    portPtr->close();
  }

  portPtr = new QSerialPort(this);
  portPtr->setPortName(_portName);
  portPtr->setBaudRate(_baudRate);
  portPtr->setDataBits(QSerialPort::Data8);
  portPtr->setParity(QSerialPort::NoParity);
  portPtr->setStopBits(QSerialPort::OneStop);
  portPtr->setFlowControl(QSerialPort::NoFlowControl);

  if (!portPtr->open(QIODevice::ReadWrite)) {
    _logger.eFmt("Failed to open %s", _portName.toStdString().c_str());
    return;
  }

  _logger.dFmt("%s opened successfully", _portName.toStdString().c_str());
}

void SerialPortObserver::write(const QByteArray data) {}

void SerialPortObserver::close() {
  if (portPtr && portPtr->isOpen()) {
    portPtr->close();
    _logger.dFmt("%s closed", _portName.toStdString().c_str());
  }
}

bool SerialPortObserver::isOpen() const { return portPtr && portPtr->isOpen(); }

// 需要改为通过按钮控制串口开关，然后再开始读数
void SerialPortObserver::run() {
  while (!isInterruptionRequested()) {
    if (portPtr && portPtr->isOpen()) {
      if (portPtr->waitForReadyRead(100)) {
        QByteArray data = portPtr->readAll();
        if (!data.isEmpty()) {
          emit signalDataReceived(data);
        }
      }
    } else {
      QThread::msleep(100);
    }
  }
}

SerialPortObserver::~SerialPortObserver() {
  close();
  quit();
  wait();
}
