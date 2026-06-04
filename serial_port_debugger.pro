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
    SerialPortObserver.cpp \
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
    CommandItemWidget.hpp \
    DatabaseWrapper.hpp \
    DebuggerApplication.hpp \
    GlobalDefinition.hpp \
    Logger.hpp \
    MainWindow.hpp \
    SerialPortObserver.hpp \
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
