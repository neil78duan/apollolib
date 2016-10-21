#-------------------------------------------------
#
# Project created by QtCreator 2016-01-13T14:13:07
#
#-------------------------------------------------

ndsdk_dir = ../../../ndsdk
game_dir = ../..

macx:{

QMAKE_MAC_SDK = macosx10.12

DEFINES += __ND_MAC__

LIBS += -L$$ndsdk_dir/lib -L../../lib/darwin_x86_64  -liconv -L$$ndsdk_dir/lib/darwin_x86_64

LIBS += -lnd_vm_dbg -lndclient_darwin_x86_64_d
#Release:LIBS += -lnd_vm -llndclient_darwin_x86_64

}
unix:!macx{
message(BUILD LINUX!)
DEFINES += __ND_LINUX__
}
win32{
message(WIN32!)

LIBS += -L$$ndsdk_dir/lib/win64 -lndclient_s


CONFIG(debug, debug|release) {
    message(BUILD win32 -debug)
    LIBS += -lndclient_s_d
    DEFINES +=  ND_DEBUG
} else {
    message(BUILD win32 -release)
    LIBS += -lndclient_s_d
}

DEFINES += __ND_WIN__
}


DEFINES += X86_64

INCLUDEPATH += $$ndsdk_dir/include \
        $$game_dir/include \
        $$game_dir

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = apolloEditor
TEMPLATE = app


SOURCES += main.cpp\
    startdialog.cpp \
    xmldialog.cpp \
    connectdialog.cpp \
    listdialog.cpp \
    dragtree.cpp \
    workdirdialog.cpp \
    newfiledialog.cpp

HEADERS  += \
    startdialog.h \
    xmldialog.h \
    connectdialog.h \
    listdialog.h \
    dragtree.h \
    workdirdialog.h \
    newfiledialog.h \
    myqtitemctrl.h

FORMS    += \
    startdialog.ui \
    xmldialog.ui \
    connectdialog.ui \
    listdialog.ui \
    workdirdialog.ui \
    newfiledialog.ui

# logic compile modules
 
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

#cli-common
SOURCES += \
    ../../cli_common/apollo_robort.cpp\
    ../../cli_common/netui_atl.cpp \
    ../../cli_common/dftCliMsgHandler.cpp \
    ../../cli_common/login_apollo.cpp

HEADERS  += ../../cli_common/dftCliMsgHandler.h \
    ../../cli_common/login_apollo.h \
    ../../cli_common/apollo_robort.h	\
    ../../cli_common/netui_atl.h

# attribute 

SOURCES += \
    ../../attribute/roleattr_help.cpp	\
    ../../attribute/stat_machine.cpp \
    ../../attribute/attr_mgr.cpp	   \
    ../../attribute/stat_data.cpp
    
HEADERS  += ../../attribute/stat_data.h \
    ../../attribute/stat_machine.h \
    ../../attribute/attr_mgr.h \
    ../../attribute/roleattr_help.h
