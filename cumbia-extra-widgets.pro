#-------------------------------------------------
#
# Project created by QtCreator 2019-05-09T16:46:14
#
#-------------------------------------------------

include(cumbia-extra-widgets.pri)

QT       += widgets

VERSION_HEX = 0x01000
VERSION = 1.0.0

TARGET = $${cumbia_extra_widgets_LIB}
TEMPLATE = lib

DEFINES -= QT_NO_DEBUG_OUTPUT

DEFINES += CUMBIAEXTRAWIDGETS_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        src/twodrwsurface.cpp

HEADERS += \
        src/twodrwsurface.h \
        src/cumbia-extra-widgets_global.h

DISTFILES += \
    cumbia-extra-widgets.pri

unix {

    doc.commands = \
    doxygen \
    Doxyfile;

    doc.files = doc/*
    doc.path = $${CUMBIA_EXTRA_WIDGETS_DOCDIR}
    QMAKE_EXTRA_TARGETS += doc

    inc.files = $${HEADERS}
    inc.path = $${CUMBIA_EXTRA_WIDGETS_INCLUDES}

    other_inst.files = $${DISTFILES}
    other_inst.path = $${CUMBIA_EXTRA_WIDGETS_INCLUDES}

    target.path = $${CUMBIA_EXTRA_WIDGETS_LIBDIR}
    INSTALLS += target inc other_inst

#message("=====================================")
#message("DOC INSTALL SKIPPED!!!!!!!!!!!!!!!!")
#message("=====================================")

    !android-g++ {
        INSTALLS += doc
    }
} # unix

# generate pkg config file
CONFIG += create_pc create_prl no_install_prl

    QMAKE_PKGCONFIG_NAME = $${cumbia_extra_widgets_LIB}
    QMAKE_PKGCONFIG_DESCRIPTION = extra controls widgets for graphical interfaces over cumbia
    QMAKE_PKGCONFIG_PREFIX = $${INSTALL_ROOT}
    QMAKE_PKGCONFIG_LIBDIR = $${target.path}
    QMAKE_PKGCONFIG_INCDIR = $${inc.path}
    QMAKE_PKGCONFIG_VERSION = $${VERSION}
    QMAKE_PKGCONFIG_DESTDIR = pkgconfig

# remove ourselves from -l (.pri)
LIBS -= -l$${cumbia_extra_widgets_LIB}
