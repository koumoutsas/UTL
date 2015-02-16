#-------------------------------------------------
#
# Project created by QtCreator 2013-08-02T13:15:33
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_GraphUnitTest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
QMAKE_CXXFLAGS += -Wall -Werror -std=c++11

SOURCES += tst_GraphUnitTest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Graph/release/ -lGraph
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Graph/debug/ -lGraph
else:unix: LIBS += -L$$OUT_PWD/../Graph/ -lGraph

INCLUDEPATH += $$PWD/../Graph
DEPENDPATH += $$PWD/../Graph

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../SetOperations/release/ -lSetOperations
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../SetOperations/debug/ -lSetOperations
else:unix: LIBS += -L$$OUT_PWD/../SetOperations/ -lSetOperations

INCLUDEPATH += $$PWD/../SetOperations
DEPENDPATH += $$PWD/../SetOperations
