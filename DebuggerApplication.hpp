#ifndef DEBUGGERAPPLICATION_HPP
#define DEBUGGERAPPLICATION_HPP

#pragma once
#include <QApplication>

#include "MainWindow.hpp"

class DebuggerApplication : public QApplication {
  Q_OBJECT

public:
  DebuggerApplication(int &argc, char **argv);
  ~DebuggerApplication();

private:
  MainWindow *mainWindowPtr;

  void initMainWindow();
};

#endif // DEBUGGERAPPLICATION_HPP
