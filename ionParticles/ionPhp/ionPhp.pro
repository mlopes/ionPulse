#-------------------------------------------------
#
# Project created by QtCreator 2011-07-15T20:56:55
#
#-------------------------------------------------
include (../../ionPulse.pri)


TEMPLATE = lib
CONFIG += core plugin
VERSION = 0.1.0

QT       += core
TARGET = ionPhp
INCLUDEPATH +=  ../..
LIBS += -L../ionEditor \
            -lionEditor \
            -lantlr3c

HEADERS += \
    plugin.h \
    editorwidgetfactory.h \
    highlighter.h \
    phpparser.h \
    phpParser/ionParserLib.h


SOURCES += \
    plugin.cpp \
    editorwidgetfactory.cpp \
    highlighter.cpp \
    phpparser.cpp \
    phpParser/ionParserLib.cpp


target.path += "$${INSTALL_DIR}/plugins/"
INSTALLS += target





















