# -------------------------------------------------
# Project created by QtCreator 2011-08-24T18:22:21
# -------------------------------------------------
TARGET = desktop
TEMPLATE = app
QWT_ROOT = .
include( $${QWT_ROOT}/qwtconfig.pri )
SUFFIX_STR = 
VVERSION = $$[QT_VERSION]
isEmpty(VVERSION) { 
    # Qt 3
    debug:SUFFIX_STR = $${DEBUG_SUFFIX}
    else:SUFFIX_STR = $${RELEASE_SUFFIX}
}
else { 
    CONFIG(debug, debug|release):SUFFIX_STR = $${DEBUG_SUFFIX}
    else:SUFFIX_STR = $${RELEASE_SUFFIX}
}
QWTLIB = qwt$${SUFFIX_STR}
isEmpty(VVERSION) { 
    # Qt 3
    DEPENDPATH += /usr/include/qwt-qt3
    INCLUDEPATH += /usr/include/qwt-qt3
    LIBS += -L$${QWT_ROOT}/lib \
        -l$${QWTLIB}-qt3
}
else { 
    # Qt 4
    DEPENDPATH += /usr/include/qwt-qt4
    INCLUDEPATH += /usr/include/qwt
    LIBS += -L$${QWT_ROOT}/lib \
        -l$${QWTLIB}
}
win32 { 
    contains(CONFIG, QwtDll) { 
        DEFINES += QT_DLL \
            QWT_DLL
        QWTLIB = $${QWTLIB}$${VER_MAJ}
    }
    win32-msvc:LIBS += $${QWT_ROOT}/lib/$${QWTLIB}.lib
    win32-msvc.net:LIBS += $${QWT_ROOT}/lib/$${QWTLIB}.lib
    win32-msvc2002:LIBS += $${QWT_ROOT}/lib/$${QWTLIB}.lib
    win32-msvc2003:LIBS += $${QWT_ROOT}/lib/$${QWTLIB}.lib
    win32-msvc2005:LIBS += $${QWT_ROOT}/lib/$${QWTLIB}.lib
    win32-msvc2008:LIBS += $${QWT_ROOT}/lib/$${QWTLIB}.lib
    win32-g++:LIBS += -L$${QWT_ROOT}/lib \
        -l$${QWTLIB}
}
SOURCES += main.cpp \
    desktop_osc.cpp \
    uart.cpp
HEADERS += desktop_osc.h \
    uart.h
FORMS += desktop_osc.ui
