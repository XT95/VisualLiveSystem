#-------------------------------------------------
#
# Project created by QtCreator 2014-10-28T22:34:42
#
#-------------------------------------------------

QT       += core gui opengl xml

TARGET = SceneEditor
TEMPLATE = app

RC_FILE = ressource.rc

INCLUDEPATH += C:/Coding/Librarys/include
LIBS        += -LC:/Coding/Librarys/lib/32


LIBS += -lwinmm -lole32 -lGLEW32 -ldsound


SOURCES += main.cpp\
        mainwindow.cpp \
    texture.cpp \
    shader.cpp \
    scene.cpp \
    fbo.cpp \
    fast2dquad.cpp \
    core.cpp \
    renderwidget.cpp \
    texteditor.cpp \
    bottomwidget.cpp \
    paramwidget.cpp \
    editorwidget.cpp \
    highlighter.cpp \
    newwidget.cpp \
    openwidget.cpp \
    newshader.cpp \
    aboutwidget.cpp

HEADERS  += mainwindow.h \
    texture.h \
    shader.h \
    scene.h \
    fbo.h \
    fast2dquad.h \
    core.h \
    renderwidget.h \
    texteditor.h \
    bottomwidget.h \
    paramwidget.h \
    editorwidget.h \
    highlighter.h \
    newwidget.h \
    openwidget.h \
    newshader.h \
    aboutwidget.h

FORMS    += mainwindow.ui \
    bottomwidget.ui \
    paramwidget.ui \
    editorwidget.ui \
    newwidget.ui \
    openwidget.ui \
    newshader.ui \
    aboutwidget.ui

RESOURCES += \
    res.qrc \
    qdarkstyle/style.qrc
