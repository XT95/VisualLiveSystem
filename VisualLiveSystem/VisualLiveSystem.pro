#-------------------------------------------------
#
# Project created by QtCreator 2013-07-12T18:33:16
#
#-------------------------------------------------

QT       += core gui opengl xml

TARGET = VisualLiveSystem
TEMPLATE = app

RC_FILE = ressource.rc

INCLUDEPATH += "../depends/rtaudio" "../depends/glew-1.13.0/include" "../depends/portmidi/porttime" "../depends/portmidi/pm_common"

debug {
LIBS += -L"../depends/lib" -lrtaudio_staticd -lportmidi_sd -lwinmm -lole32 -lglew32 -ldsound -ladvapi32 -luser32 -lopengl32
}

release {
LIBS += -L"../depends/lib" -lrtaudio_static -lportmidi_s -lwinmm -lole32 -lglew32 -ldsound -ladvapi32 -luser32 -lopengl32
}

SOURCES += main.cpp\
        mainwindow.cpp \
    renderwidget.cpp \
    scene.cpp \
    shader.cpp \
    fast2dquad.cpp \
    fbo.cpp \
    channelwidget.cpp \
    visualmanager.cpp \
    core.cpp \
    masterwidget.cpp \
    glscene.cpp \
    projectorwidget.cpp \
    loadingwidget.cpp \
    texture.cpp \
    soundmanager.cpp \
    midiwindow.cpp \
    qmidisequencer.cpp \
    transition.cpp \
    FFTAnalyzer.cpp \
    audiovisualizer.cpp \
    signal/decoder.cpp \
    signal/signal.cpp \
    signal/basserrorhandler.cpp \
    signal/audiopipe.cpp \
    signal/fft.cpp  \
    signal/bassproc.c

HEADERS  += mainwindow.h \
    renderwidget.h \
    scene.h \
    shader.h \
    fast2dquad.h \
    fbo.h \
    channelwidget.h \
    visualmanager.h \
    core.h \
    masterwidget.h \
    glscene.h \
    projectorwidget.h \
    loadingwidget.h \
    texture.h \
    soundmanager.h \
    midiwindow.h \
    qmidisequencer.h \
    transition.h \
    FFTAnalyzer.h \
    audiovisualizer.h \
    signal/decoder.hpp \
    signal/signal.hpp \
    signal/basserrorhandler.hpp \
    signal/audiopipe.hpp \
    signal/fft.hpp \
    signal/bassproc.h

FORMS    += mainwindow.ui \
    channelwidget.ui \
    visualmanager.ui \
    masterwidget.ui \
    loadingwidget.ui \
    midiwindow.ui \
    soundmanager.ui

RESOURCES += \
    res.qrc \
    qdarkstyle/style.qrc

OTHER_FILES += \
    res/shaders/open.glsl \
    res/shaders/postprocess.glsl
