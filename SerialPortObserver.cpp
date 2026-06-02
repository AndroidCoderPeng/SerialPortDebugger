#include "SerialPortObserver.hpp"

#include <QSerialPort>

#ifdef _WIN32
#define PortName "COM1"
#else
// Linux, macOS 等
#define PortName "dev/ttyS0"
#endif
#define BaudRate 19200

std::unique_ptr<SerialPortObserver> SerialPortObserver::_instance = nullptr;

SerialPortObserver::SerialPortObserver() : _logger("SerialPortObserver") {
  _logger.i("initialized succeed.");
}

SerialPortObserver *SerialPortObserver::get() {
  if (!_instance) {
    _instance = std::unique_ptr<SerialPortObserver>(new SerialPortObserver());
    _instance->start();
  }
  return _instance.get();
}

// 需要改为通过按钮控制串口开关，然后再开始读数
void SerialPortObserver::run() {
  QSerialPort serial;
  serial.setPortName(PortName);
  serial.setBaudRate(BaudRate);
  serial.setDataBits(QSerialPort::Data8);
  serial.setParity(QSerialPort::NoParity);
  serial.setStopBits(QSerialPort::OneStop);
  serial.setFlowControl(QSerialPort::NoFlowControl);

  if (!serial.open(QIODevice::ReadOnly)) {
    _logger.e("Failed to open serial port");
    return;
  }

  _logger.dFmt("%s opened successfully", PortName);

  while (!isInterruptionRequested()) {
    if (serial.waitForReadyRead(100)) {
      QByteArray data = serial.readAll();
      if (!data.isEmpty()) {
        emit signalDataReceived(data);
      }
    }
  }
  serial.close();
  _logger.i("Serial port closed");
}

SerialPortObserver::~SerialPortObserver() {
  quit();
  wait();
}
