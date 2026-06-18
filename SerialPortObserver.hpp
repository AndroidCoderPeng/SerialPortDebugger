#ifndef SERIALPORTOBSERVER_HPP
#define SERIALPORTOBSERVER_HPP

#include <QThread>

// 如果串口的生命期和应用本身保持一致，只读取不写入，不涉及跨线程操作，那么直接继承QThread就是合理的
class SerialPortObserver : public QThread {
  Q_OBJECT

public:
  explicit SerialPortObserver(QObject *parent = nullptr);
  ~SerialPortObserver();

  static inline SerialPortObserver *get() {
    static SerialPortObserver instance;
    return &instance;
  }

signals:
  void signalDataReceived(const QByteArray &data);

private:
  void run() override;
};

#endif // SERIALPORTOBSERVER_HPP
