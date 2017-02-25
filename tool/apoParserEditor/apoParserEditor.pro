#-------------------------------------------------
#
# Project created by QtCreator 2017-02-10T11:22:45
#
#-------------------------------------------------

ndsdk_dir = ../../../ndsdk
apollo_dir = ../..

macx:{

QMAKE_MAC_SDK = macosx10.12

DEFINES += __ND_MAC__

LIBS += -L$$ndsdk_dir/lib -L../../lib/darwin_x86_64  -liconv -L$$ndsdk_dir/lib/darwin_x86_64


CONFIG(debug, debug|release) {
    message(BUILD macOs -debug)
    DEFINES +=  ND_DEBUG
    LIBS += -lnd_vm_dbg -lndclient_darwin_x86_64_d
} else {
    message(BUILD macOs -release)
    LIBS += -lnd_vm -lndclient_darwin_x86_64
}


}
# end mac
unix:!macx{
message(BUILD LINUX!)
DEFINES += __ND_LINUX__
}
win32{
message(WIN32!)

LIBS += -L$$ndsdk_dir/lib/win64

CONFIG(debug, debug|release) {
    message(BUILD win32 -debug)
    LIBS += -lndclient_s_d
    DEFINES +=  ND_DEBUG
} else {
    message(BUILD win32 -release)
    LIBS += -lndclient_s
}

DEFINES += __ND_WIN__
}


DEFINES += X86_64

INCLUDEPATH += $$ndsdk_dir/include \
        $$apollo_dir/include \
        $$apollo_dir

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = apoParserEditor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    src/apoUiExeNode.cpp \
    src/apoUiMainEditor.cpp \
    src/apoUiBezier.cpp \
    src/apoBaseExeNode.cpp

HEADERS  += mainwindow.h \
    src/apoUiExeNode.h \
    src/apoUiMainEditor.h \
    src/apoUiBezier.h \
    src/apouinodedef.h \
    src/apoBaseExeNode.h


SOURCES += \
    ../../logic_parser/logicDataType.cpp \
    ../../logic_parser/logicEngineRoot.cpp \
    ../../logic_parser/logicParser.cpp \
    ../../logic_parser/logic_compile.cpp \
    ../../logic_parser/logic_editor_helper.cpp \
    ../../logic_parser/objectBaseMgr.cpp \
    ../../logic_parser/dbl_mgr.cpp \
    ../../logic_parser/logic_function.cpp \
    ../../logic_parser/dbldata2netstream.cpp \
    ../../logic_parser/logicStruct.cpp

HEADERS  += ../../logic_parser/dbl_mgr.h \
    ../../logic_parser/logicDataType.h \
    ../../logic_parser/logicEngineRoot.h \
    ../../logic_parser/logicParser.h \
    ../../logic_parser/logic_compile.h \
    ../../logic_parser/logic_editor_helper.h \
    ../../logic_parser/objectBaseMgr.h \
    ../../logic_parser/logic_function.h \
    ../../logic_parser/dbldata2netstream.h \
    ../../logic_parser/logicStruct.hpp

FORMS    += mainwindow.ui
