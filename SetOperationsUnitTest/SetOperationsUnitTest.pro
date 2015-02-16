#-------------------------------------------------
#
# Project created by QtCreator 2013-07-31T12:13:28
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_SetOperationsUnitTest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
QMAKE_CXXFLAGS += -Wall -Werror -std=c++11

SOURCES += tst_SetOperationsUnitTest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../SetOperations/release/ -lSetOperations
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../SetOperations/debug/ -lSetOperations
else:unix: LIBS += -L$$OUT_PWD/../SetOperations/ -lSetOperations

INCLUDEPATH += $$PWD/../SetOperations
DEPENDPATH += $$PWD/../SetOperations
