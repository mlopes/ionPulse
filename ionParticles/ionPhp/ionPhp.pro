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
            -ldparse

HEADERS += \
    plugin.h \
    editorwidgetfactory.h \
    highlighter.h \
    phpParser/ionPhp.g \
    phpparser.h


SOURCES += \
    plugin.cpp \
    editorwidgetfactory.cpp \
    highlighter.cpp \
    ionPhp.g.d_parser.cpp \
    phpparser.cpp


ionPhp_d_parser.target = $${PWD}/phpParser/ionPhp.g.d_parser.cpp
ionPhp_d_parser.commands =  cd "$${PWD}/phpParser/" && /usr/local/bin/make_dparser -I -o ionPhp.g.d_parser.cpp ionPhp.g
ionPhp_d_parser.depends =  $${PWD}/phpParser/ionPhp.g
QMAKE_EXTRA_TARGETS += ionPhp_d_parser

ionPhp_d_parser_dep.target = ionPhp.g.d_parser.cpp
ionPhp_d_parser_dep.commands =  $$QMAKE_COPY $${PWD}/phpParser/ionPhp.g.d_parser.cpp  $${OUT_PWD}/ionPhp.g.d_parser.cpp
ionPhp_d_parser_dep.depends =  $${PWD}/phpParser/ionPhp.g.d_parser.cpp
QMAKE_EXTRA_TARGETS += ionPhp_d_parser_dep


target.path += "$${INSTALL_DIR}/plugins/"
INSTALLS += target







