#include "SerialPortObserver.hpp"

#include <QSerialPort>

#ifdef _WIN32
#define PortName "COM1"
#else
#define PortName "/dev/ttyS0"
#endif
#define BaudRate 19200

SerialPortObserver::SerialPortObserver()
    : QThread(), _logger("SerialPortObserver") {
  this->start();
  _logger.i("initialized succeed.");
}

// bool SerialPortObserver::open(const QString &portName, qint32 baudRate,
//                               const QString &dataBits, const QString &parity,
//                               const QString &stopBits,
//                               const QString &flowControl) {
//   _portName = portName;
//   _baudRate = baudRate;

//   if (portPtr && portPtr->isOpen()) {
//     portPtr->close();
//   }

//   portPtr = new QSerialPort(this);
//   portPtr->setPortName(_portName);
//   portPtr->setBaudRate(_baudRate);

//   // 设置数据位
//   if (dataBits == "5") {
//     portPtr->setDataBits(QSerialPort::Data5);
//   } else if (dataBits == "6") {
//     portPtr->setDataBits(QSerialPort::Data6);
//   } else if (dataBits == "7") {
//     portPtr->setDataBits(QSerialPort::Data7);
//   } else {
//     portPtr->setDataBits(QSerialPort::Data8); // 默认 8
//   }

//   // 设置校验位
//   if (parity == "Even") {
//     portPtr->setParity(QSerialPort::EvenParity);
//   } else if (parity == "Odd") {
//     portPtr->setParity(QSerialPort::OddParity);
//   } else {
//     portPtr->setParity(QSerialPort::NoParity); // 默认无
//   }

//   // 设置停止位
//   if (stopBits == "2") {
//     portPtr->setStopBits(QSerialPort::TwoStop);
//   } else {
//     portPtr->setStopBits(QSerialPort::OneStop); // 默认 1
//   }

//   // 设置流控
//   if (flowControl == "Hardware") {
//     portPtr->setFlowControl(QSerialPort::HardwareControl);
//   } else if (flowControl == "Software") {
//     portPtr->setFlowControl(QSerialPort::SoftwareControl);
//   } else {
//     portPtr->setFlowControl(QSerialPort::NoFlowControl); // 默认无
//   }

//   if (!portPtr->open(QIODevice::ReadWrite)) {
//     _logger.eFmt("Failed to open %s", _portName.toStdString().c_str());
//     return false;
//   }

//   _logger.dFmt("%s opened successfully", _portName.toStdString().c_str());
//   return true;
// }

// void SerialPortObserver::write(const QByteArray data) {
//   if (portPtr && portPtr->isOpen()) {
//     portPtr->write(data);
//   }
// }

// void SerialPortObserver::close() {
//   if (portPtr && portPtr->isOpen()) {
//     portPtr->close();
//     _logger.dFmt("%s closed", _portName.toStdString().c_str());
//   }
// }

// bool SerialPortObserver::isOpen() const { return portPtr &&
// portPtr->isOpen(); }

// 需要改为通过按钮控制串口开关，然后再开始读数
void SerialPortObserver::run() {
  QSerialPort serialPort;
  serialPort.setPortName(PortName);
  serialPort.setBaudRate(BaudRate);
  serialPort.setDataBits(QSerialPort::Data8);
  serialPort.setParity(QSerialPort::NoParity);
  serialPort.setStopBits(QSerialPort::OneStop);
  serialPort.setFlowControl(QSerialPort::NoFlowControl);

  while (!isInterruptionRequested()) {
    if (!serialPort.isOpen()) {
      if (!serialPort.open(QIODevice::ReadOnly)) {
        _logger.eFmt("Failed to open %s: %s", PortName,
                     serialPort.errorString().toStdString().c_str());
        msleep(1000);
        continue;
      }
      _logger.dFmt("%s opened successfully, baud %d", PortName, BaudRate);
    }

    if (serialPort.waitForReadyRead(200)) {
      QByteArray data = serialPort.readAll();
      while (serialPort.waitForReadyRead(10)) {
        data += serialPort.readAll();
      }

      if (!data.isEmpty()) {
        emit signalDataReceived(data);
      }
      continue;
    }

    if (isInterruptionRequested()) {
      break;
    }

    const QSerialPort::SerialPortError err = serialPort.error();
    if (err == QSerialPort::NoError || err == QSerialPort::TimeoutError) {
      continue;
    }

    _logger.eFmt("Serial error: %s",
                 serialPort.errorString().toStdString().c_str());

    serialPort.close();
    msleep(300);
  }

  if (serialPort.isOpen()) {
    serialPort.close();
    _logger.dFmt("%s closed", PortName);
  }
}

SerialPortObserver::~SerialPortObserver() {
  requestInterruption();
  wait();
}
