# -------------------------------------------------
# Project created by QtCreator 2011-08-24T18:22:21
# -------------------------------------------------
TARGET = desktop
TEMPLATE = app
QWT_ROOT = .
include( $${QWT_ROOT}/qwtconfig.pri )
SUFFIX_STR = 
VVERSION = $$[QT_VERSION]

CONFIG(debug, debug|release):SUFFIX_STR = $${DEBUG_SUFFIX}
else:SUFFIX_STR = $${RELEASE_SUFFIX}
QWTLIB = qwt$${SUFFIX_STR}

DEPENDPATH += /usr/include/qwt-qt4
INCLUDEPATH += /usr/include/qwt
LIBS += -L$${QWT_ROOT}/lib
SOURCES += main.cpp \
    desktop_osc.cpp \
    uart.cpp \
    desktop_osc_device.cpp \
    desktop_osc_graph.cpp
HEADERS += desktop_osc.h \
    uart.h
FORMS += desktop_osc.ui


INCLUDEPATH += /usr/include /usr/local/include /usr/include/qwt-qt4 /usr/include/qt4/QtExtSerialPort ../../arduino/quadrocopter_oop/

LIBS += -L/usr/local/lib -lqwt-qt4
LIBS += -L/usr/local/lib -lQtSvg

CONFIG += extserialport qwt

# magick?
LIBS += -L/usr/lib/x86_64-linux-gnu/ -lqextserialport-1.2
