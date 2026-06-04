TARGET = SerialPortDebugger
TEMPLATE = app
win32 {
    RC_ICONS = application.ico
}

QT       += core gui widgets serialport sql

CONFIG += c++11

SOURCES += \
    CommandItemWidget.cpp \
    DatabaseWrapper.cpp \
    DebuggerApplication.cpp \
    Logger.cpp \
    MainWindow.cpp \
    SaveCommandDialog.cpp \
    SerialPortManager.cpp \
    SerialPortObserver.cpp \
    TaskExecutor.cpp \
    Utils.cpp \
    commandscriptdialog.cpp \
    main.cpp

HEADERS += \
    CommandItemWidget.hpp \
    DatabaseWrapper.hpp \
    DebuggerApplication.hpp \
    GlobalDefinition.hpp \
    Logger.hpp \
    MainWindow.hpp \
    SaveCommandDialog.hpp \
    SerialPortManager.hpp \
    SerialPortObserver.hpp \
    TaskExecutor.hpp \
    Utils.hpp \
    commandscriptdialog.hpp

FORMS += \
    MainWindow.ui \
    SaveCommandDialog.ui \
    commandscriptdialog.ui

RESOURCES += \
    font.qrc \
    image.qrc
