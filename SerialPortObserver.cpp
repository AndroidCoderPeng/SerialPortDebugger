#include "SerialPortObserver.hpp"

#include <QSerialPort>

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

void SerialPortObserver::run() {
  while (!isInterruptionRequested()) {
    emit signalDataReceived(QByteArray("Hello from SerialPortObserver!"));
  }
}

SerialPortObserver::~SerialPortObserver() {
  quit();
  wait();
}
