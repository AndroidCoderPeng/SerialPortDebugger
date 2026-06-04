TARGET = SerialPortDebugger
TEMPLATE = app
win32 {
    RC_ICONS = application.ico
}

QT       += core gui widgets serialport sql

CONFIG += c++11

SOURCES += \
    CommandItemWidget.cpp \
    CommandScriptDialog.cpp \
    DatabaseWrapper.cpp \
    DebuggerApplication.cpp \
    Logger.cpp \
    MainWindow.cpp \
    SaveCommandDialog.cpp \
    SerialPortManager.cpp \
    SerialPortObserver.cpp \
    TaskExecutor.cpp \
    Utils.cpp \
    main.cpp

HEADERS += \
    CommandItemWidget.hpp \
    CommandScriptDialog.hpp \
    DatabaseWrapper.hpp \
    DebuggerApplication.hpp \
    GlobalDefinition.hpp \
    Logger.hpp \
    MainWindow.hpp \
    SaveCommandDialog.hpp \
    SerialPortManager.hpp \
    SerialPortObserver.hpp \
    TaskExecutor.hpp \
    Utils.hpp

FORMS += \
    CommandScriptDialog.ui \
    MainWindow.ui \
    SaveCommandDialog.ui

RESOURCES += \
    font.qrc \
    image.qrc
