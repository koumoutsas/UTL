#-------------------------------------------------
#
# Project created by QtCreator 2013-08-02T13:00:00
#
#-------------------------------------------------

QT       -= core gui

TARGET = Graph
TEMPLATE = lib
QMAKE_CXXFLAGS += -Wall -Werror -std=c++11

DEFINES += GRAPH_LIBRARY

SOURCES +=

HEADERS += \
    UndirectedGraph.h \
    KCore.h \
    DepthFirstVisitor.h \
    IncreasingUndirectedGraph.h \
    GraphTraversalVisitor.h \
    BreadthFirstVisitor.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../SetOperations/release/ -lSetOperations
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../SetOperations/debug/ -lSetOperations
else:unix: LIBS += -L$$OUT_PWD/../SetOperations/ -lSetOperations

INCLUDEPATH += $$PWD/../SetOperations
DEPENDPATH += $$PWD/../SetOperations
