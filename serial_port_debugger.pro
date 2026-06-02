TARGET = SerialPortDebugger
TEMPLATE = app
win32 {
    RC_ICONS = application.ico
}

QT       += core gui widgets serialport sql

CONFIG += c++11

SOURCES += \
    DebuggerApplication.cpp \
    Logger.cpp \
    MainWindow.cpp \
    com_message.cpp \
    combo_box_item_delegate.cpp \
    command.cpp \
    commandscriptdialog.cpp \
    main.cpp \
    savecommanddialog.cpp \
    scriptconfig.cpp \
    taskexecutor.cpp \
    utils.cpp

HEADERS += \
    DebuggerApplication.hpp \
    Logger.hpp \
    MainWindow.hpp \
    com_message.hpp \
    combo_box_item_delegate.hpp \
    command.hpp \
    commandscriptdialog.hpp \
    savecommanddialog.hpp \
    scriptconfig.hpp \
    taskexecutor.hpp \
    utils.hpp

FORMS += \
    MainWindow.ui \
    commandscriptdialog.ui \
    savecommanddialog.ui

RESOURCES += \
    font.qrc \
    image.qrc
