# + ----------------------------------------------------------------- +
#
# Customization section:
#
# Customize the following paths according to your installation:
#
#
# Here cumbia-extra-widgets will be installed
# INSTALL_ROOT can be specified from the command line running qmake "INSTALL_ROOT=/my/install/path"
#

isEmpty(INSTALL_ROOT) {
    INSTALL_ROOT = /usr/local/cumbia-libs
}

#
#
# Here cumbia-extra-widgets include files will be installed
    CUMBIA_EXTRA_WIDGETS_INCLUDES=$${INSTALL_ROOT}/include/cumbia-extra-widgets
#
#
# Here cumbia-extra-widgets share files will be installed
#
    CUMBIA_EXTRA_WIDGETS_SHARE=$${INSTALL_ROOT}/share/cumbia-extra-widgets
#
#
# Here cumbia-extra-widgets libraries will be installed
    CUMBIA_EXTRA_WIDGETS_LIBDIR=$${INSTALL_ROOT}/lib
#
#
# Here cumbia-extra-widgets documentation will be installed
    CUMBIA_EXTRA_WIDGETS_DOCDIR=$${INSTALL_ROOT}/share/doc/cumbia-extra-widgets
#
# The name of the library
    cumbia_extra_widgets_LIB=cumbia-extra-widgets$${QTVER_SUFFIX}
#
#

unix:!android-g++ {
    CONFIG += link_pkgconfig
    PKGCONFIG += cumbia cumbia-qtcontrols$${QTVER_SUFFIX}
}

QT       += widgets

VERSION_HEX = 0x01000
VERSION = 1.0.0


LIBS += -L$${INSTALL_ROOT}/lib -l$${cumbia_extra_widgets_LIB}
