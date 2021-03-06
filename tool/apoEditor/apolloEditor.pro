#-------------------------------------------------
#
# Project created by QtCreator 2016-01-13T14:13:07
#
#-------------------------------------------------

ndsdk_dir = ../../../ndsdk
apolib_dir = ../..
apoBP_dir = ../../../nodeflow

CONFIG(debug, debug|release) {
    message(BUILD win32 -debug)
    LIBS += -lndclient_s_d -lnodeflow_s_d
    DEFINES +=  ND_DEBUG
} else {
    message(BUILD win32 -release)
    LIBS += -lndclient_s -lnodeflow_s
}

LIBS +=  -lapoBluePrint

macx:{

    QMAKE_MAC_SDK = macosx10.14
    DEFINES += __ND_MAC__
    platform_name = darwin_x86_64

    LIBS += -liconv

    TARGET = ../../editor/apolloEditor
}

unix:!macx{
    message(BUILD LINUX!)
    DEFINES += __ND_LINUX__
    platform_name = linux_x86_64
    LIBS += -liconv

    TARGET = ../../editor/apolloEditor
}

win32{
    message(WIN32!)
    platform_name = Win64
    DEFINES += __ND_WIN__ DO_NOT_CONVERT_PRINT_TEXT
    TARGET = ../../../editor/apolloEditor
}



libsub = lib/$$platform_name
LIBS += -L$$ndsdk_dir/lib  -L$$ndsdk_dir/$$libsub  -L$$apoBP_dir/$$libsub

DEFINES += X86_64

INCLUDEPATH += $$ndsdk_dir/include \
        $$apolib_dir/include \
        $$apolib_dir  ./include ./src \
        $$apoBP_dir/include $$apoBP_dir/apoScript


#-------qt----------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app


SOURCES += src/main.cpp\
    src/startdialog.cpp \
    src/connectdialog.cpp \
    ../../game_parser/apoGameCommon.cpp \
    ../../game_parser/dbl_mgr.cpp \
    ../../game_parser/dbl_plugin.cpp \
    ../../game_parser/dbldata2netstream.cpp \
    $$apolib_dir/cli_common/apollo_robort.cpp\
    $$apolib_dir/cli_common/dftCliMsgHandler.cpp \
    $$apolib_dir/cli_common/login_apollo.cpp    \
    $$apolib_dir/attribute/roleattr_help.cpp	\
    $$apolib_dir/attribute/stat_machine.cpp \
    $$apolib_dir/attribute/attr_mgr.cpp	   \
    $$apolib_dir/attribute/stat_data.cpp

win32{
    SOURCES +=  $$apoBP_dir/src/winPlug/export2Exe.cpp \
    $$apoBP_dir/src/winPlug/pelib.cpp \
    $$apoBP_dir/src/winPlug/peliberr.cpp
 

    HEADERS += $$apoBP_dir/src/winPlug/pelib.h \
    $$apoBP_dir/src/winPlug/peliberr.h
}

macx:{
    SOURCES += $$apoBP_dir/src/machoPlug/machoLoader.cpp

    HEADERS += $$apoBP_dir/src/machoPlug/machoLoader.h \
    $$apoBP_dir/src/machoPlug/nfsection_def.h
}


HEADERS  += \
    src/startdialog.h \
    src/connectdialog.h \
    ../../game_parser/apoGameCommon.h \
    ../../game_parser/dbl_mgr.h \
    ../../game_parser/dbldata2netstream.h \
    $$apolib_dir/cli_common/dftCliMsgHandler.h \
    $$apolib_dir/cli_common/login_apollo.h \
    $$apolib_dir/cli_common/apollo_robort.h \
    $$apolib_dir/attribute/stat_data.h \
    $$apolib_dir/attribute/stat_machine.h \
    $$apolib_dir/attribute/attr_mgr.h \
    $$apolib_dir/attribute/roleattr_help.h

FORMS    += \
    src/startdialog.ui \
    src/connectdialog.ui

