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
    SerialPortManager.cpp \
    SerialPortObserver.cpp \
    TaskExecutor.cpp \
    Utils.cpp \
    com_message.cpp \
    commandscriptdialog.cpp \
    main.cpp \
    savecommanddialog.cpp

HEADERS += \
    CommandItemWidget.hpp \
    DatabaseWrapper.hpp \
    DebuggerApplication.hpp \
    GlobalDefinition.hpp \
    Logger.hpp \
    MainWindow.hpp \
    SerialPortManager.hpp \
    SerialPortObserver.hpp \
    TaskExecutor.hpp \
    Utils.hpp \
    com_message.hpp \
    commandscriptdialog.hpp \
    savecommanddialog.hpp

FORMS += \
    MainWindow.ui \
    commandscriptdialog.ui \
    savecommanddialog.ui

RESOURCES += \
    font.qrc \
    image.qrc
