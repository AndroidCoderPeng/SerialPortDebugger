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

bool SerialPortObserver::open(const QString &portName, qint32 baudRate,
                              const QString &dataBits, const QString &parity,
                              const QString &stopBits,
                              const QString &flowControl) {
  _portName = portName;
  _baudRate = baudRate;

  if (portPtr && portPtr->isOpen()) {
    portPtr->close();
  }

  portPtr = new QSerialPort(this);
  portPtr->setPortName(_portName);
  portPtr->setBaudRate(_baudRate);

  // 设置数据位
  if (dataBits == "5") {
    portPtr->setDataBits(QSerialPort::Data5);
  } else if (dataBits == "6") {
    portPtr->setDataBits(QSerialPort::Data6);
  } else if (dataBits == "7") {
    portPtr->setDataBits(QSerialPort::Data7);
  } else {
    portPtr->setDataBits(QSerialPort::Data8); // 默认 8
  }

  // 设置校验位
  if (parity == "Even") {
    portPtr->setParity(QSerialPort::EvenParity);
  } else if (parity == "Odd") {
    portPtr->setParity(QSerialPort::OddParity);
  } else {
    portPtr->setParity(QSerialPort::NoParity); // 默认无
  }

  // 设置停止位
  if (stopBits == "2") {
    portPtr->setStopBits(QSerialPort::TwoStop);
  } else {
    portPtr->setStopBits(QSerialPort::OneStop); // 默认 1
  }

  // 设置流控
  if (flowControl == "Hardware") {
    portPtr->setFlowControl(QSerialPort::HardwareControl);
  } else if (flowControl == "Software") {
    portPtr->setFlowControl(QSerialPort::SoftwareControl);
  } else {
    portPtr->setFlowControl(QSerialPort::NoFlowControl); // 默认无
  }

  if (!portPtr->open(QIODevice::ReadWrite)) {
    _logger.eFmt("Failed to open %s", _portName.toStdString().c_str());
    return false;
  }

  _logger.dFmt("%s opened successfully", _portName.toStdString().c_str());
  return true;
}

void SerialPortObserver::write(const QByteArray data) {
  if (portPtr && portPtr->isOpen()) {
    portPtr->write(data);
  }
}

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
