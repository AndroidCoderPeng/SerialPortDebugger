#ifndef SERIALPORTOBSERVER_HPP
#define SERIALPORTOBSERVER_HPP

#include <QThread>

#include "Logger.hpp"

class SerialPortObserver : public QThread {
  Q_OBJECT

public:
  static SerialPortObserver *get();
  ~SerialPortObserver();

signals:
  void signalDataReceived(const QByteArray &data);

private:
  SerialPortObserver();

  static std::unique_ptr<SerialPortObserver> _instance;

  Logger _logger;

  void run() override;
};

#endif // SERIALPORTOBSERVER_HPP
