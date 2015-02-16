#-------------------------------------------------
#
# Project created by QtCreator 2013-07-26T16:10:51
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_KCoreUnitTest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
QMAKE_CXXFLAGS += -Wall -Werror -std=c++11

SOURCES += tst_KCoreUnitTest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Graph/release/ -lGraph
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Graph/debug/ -lGraph
else:unix: LIBS += -L$$OUT_PWD/../Graph/ -lGraph

INCLUDEPATH += $$PWD/../Graph
DEPENDPATH += $$PWD/../Graph
