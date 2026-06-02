#ifndef SERIALPORTOBSERVER_HPP
#define SERIALPORTOBSERVER_HPP

#include <QSerialPort>
#include <QThread>

#include "Logger.hpp"

class SerialPortObserver : public QThread {
  Q_OBJECT

public:
  static SerialPortObserver *get();
  ~SerialPortObserver();

  void open(const QString &portName, qint32 baudRate);
  void write(const QByteArray data);
  void close();
  bool isOpen() const;

signals:
  void signalDataReceived(const QByteArray &data);

private:
  SerialPortObserver();

  static std::unique_ptr<SerialPortObserver> _instance;

  Logger _logger;
  QSerialPort *portPtr{nullptr};
  QString _portName;
  qint32 _baudRate;

  void run() override;
};

#endif // SERIALPORTOBSERVER_HPP
