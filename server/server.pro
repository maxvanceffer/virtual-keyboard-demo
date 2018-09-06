TEMPLATE = app
TARGET = server
QT += widgets dbus
INCLUDEPATH += .

HEADERS += keyboard.h device.h
SOURCES += keyboard.cpp device.cpp main.cpp

RESOURCES += \
    resources.qrc
