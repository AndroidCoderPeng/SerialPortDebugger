#ifndef SERIALPORTMANAGER_HPP
#define SERIALPORTMANAGER_HPP

#include <QObject>
#include <QString>

class QThread;
class SerialPortWorker;

// 适用于跨线程串口管理，提供线程安全的接口，内部使用互斥锁保护串口资源，注意和SerialPortObserver区分
class SerialPortManager : public QObject {
  Q_OBJECT
public:
  explicit SerialPortManager(QObject *parent = nullptr);
  ~SerialPortManager() override;

  static inline SerialPortManager *get() {
    static SerialPortManager instance;
    return &instance;
  }

  bool open(const QString &portName, qint32 baudRate,
            const QString &dataBits = "", const QString &parity = "",
            const QString &stopBits = "", const QString &flowControl = "");

  void write(const QByteArray &data);
  void close();
  bool isOpen() const;

signals:
  void signalDataReceived(const QByteArray &data);
  void signalPortStateChanged(bool opened);
  void signalError(const QString &message);

private:
  QThread *ioThreadPtr;
  SerialPortWorker *workerPtr;
};

#endif // SERIALPORTMANAGER_HPP
