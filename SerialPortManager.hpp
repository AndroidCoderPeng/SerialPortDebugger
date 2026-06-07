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

  // 不能让 UI 层跳过 Manager 直接连 Worker。
  // 正确的做法是 Manager 暴露自己的信号，在 slot 里 re-emit。
  // 虽然看起来像是"把 Worker 的信号再写一遍"，但这是合理的外观模式，只为换取：
  //    1.UI 层不知道 Worker 的存在（解耦）
  //    2.Manager 可以在转发前做额外处理（日志、过滤、缓冲）
  //    3.未来换掉 Worker 实现时，UI 完全不受影响
signals:
  void dataReceivedSignal(const QByteArray &data);
  void stateChangedSignal(bool opened);
  void errorOccurredSignal(const QString &message);

private slots:
  void dataReceivedSlot(const QByteArray &data);
  void stateChangedSlot(bool opened);
  void errorOccurredSlot(const QString &message);

private:
  QThread *ioThreadPtr;
  SerialPortWorker *workerPtr;
};

#endif // SERIALPORTMANAGER_HPP
