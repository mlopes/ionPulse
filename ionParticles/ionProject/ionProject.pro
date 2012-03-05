#-------------------------------------------------
#
# Project created by QtCreator 2011-07-15T20:56:55
#
#-------------------------------------------------
include (../../ionPulse.pri)


TEMPLATE = lib
CONFIG += core plugin
VERSION = 0.1.0

QT       += core script
TARGET = ionProject
INCLUDEPATH +=  ../..

HEADERS += \
    plugin.h \
    treeitem.h \
    projectapi.h \
    treemodeladapter.h \
    directorytreesource.h \
    treeviewitemdelegate.h \
    treeview.h \
    treeviewpanel.h

SOURCES += \
    plugin.cpp \
    treeitem.cpp \
    treemodeladapter.cpp \
    directorytreesource.cpp \
    treeviewitemdelegate.cpp \
    treeview.cpp \
    treeviewpanel.cpp

target.path += "$${INSTALL_DIR}/plugins/"
INSTALLS += target

































