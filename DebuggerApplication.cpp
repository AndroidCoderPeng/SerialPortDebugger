#include "DebuggerApplication.hpp"
#include "SerialPortObserver.hpp"

#include <QFontDatabase>
#include <QScreen>

DebuggerApplication::DebuggerApplication(int &argc, char **argv)
    : QApplication(argc, argv), _logger("DebuggerApplication"),
      mainWindowPtr(nullptr) {

  int fontId = QFontDatabase::addApplicationFont(":/msyh.ttc");
  if (fontId != -1) {
    QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
    QFont font(fontFamily);
    setFont(font);
  } else {
    _logger.w("Failed to load font.");
  }

  initMainWindow();

  // 初始化串口读数线程
  // QObject::connect(SerialPortObserver::get(),
  //                  &SerialPortObserver::signalDataReceived, mainWindowPtr,
  //                  &MainWindow::slotDataReceived);
}

void DebuggerApplication::initMainWindow() {
  mainWindowPtr = new MainWindow();
  mainWindowPtr->setWindowTitle("串口调试助手");
  mainWindowPtr->setWindowIcon(QIcon(":/application.png"));

  const QRect rect = primaryScreen()->availableGeometry();
  mainWindowPtr->move((rect.width() - mainWindowPtr->width()) / 2,
                      (rect.height() - mainWindowPtr->height()) / 2);
  mainWindowPtr->show();
  _logger.d("MainWindow initialized and shown.");
}

DebuggerApplication::~DebuggerApplication() {
  // QThread 子对象会随 Application 销毁自动 quit/wait
  delete mainWindowPtr;
  mainWindowPtr = nullptr;
  _logger.d("DebuggerApplication is being destroyed, cleaning up resources.");
}