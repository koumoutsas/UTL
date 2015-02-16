#-------------------------------------------------
#
# Project created by QtCreator 2013-07-31T10:50:18
#
#-------------------------------------------------

QT       -= core gui

TARGET = SetOperations
TEMPLATE = lib
QMAKE_CXXFLAGS += -Wall -Werror -std=c++11

DEFINES += SETOPERATIONS_LIBRARY

SOURCES +=

HEADERS += \
    DisjointSets.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
