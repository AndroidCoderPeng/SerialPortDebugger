#include "SerialPortObserver.hpp"

#include <QSerialPort>

#ifdef _WIN32
#define PortName "COM1"
#else
#define PortName "/dev/ttyS0"
#endif
#define BaudRate 19200

SerialPortObserver::SerialPortObserver(QObject *parent)
    : QThread(), _logger("SerialPortObserver") {
  this->start();
  _logger.i("initialized succeed.");
}

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
