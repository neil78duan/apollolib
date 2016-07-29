#-------------------------------------------------
#
# Project created by QtCreator 2016-01-13T14:13:07
#
#-------------------------------------------------

ndsdk_dir = ../../../ndsdk
game_dir = ../..

macx:{
message(BUILD MACOX)
DEFINES += __MAC_OS__ ND_UNIX

LIBS += -L$$ndsdk_dir/lib -lndclient_darwin_x86_64_d \
    -L../../../lib/darwin_x86_64  -lnetMessage_d  \
    -liconv \
    -L$$ndsdk_dir/lib/darwin_x86_64 -lnd_vm_dbg


}
unix:!macx{
message(BUILD LINUX!)
DEFINES += __LINUX__ ND_UNIX
}
win32{
message(WIN32!)

LIBS += -L$$ndsdk_dir/lib/win64 -lndclient_s

SOURCES += \
    ../../netMessage/dataStream.cpp\
    ../../netMessage/MessageName.cpp\
    ../../netMessage/auto_dataType.cpp\
    ../../netMessage/dbDataConvert.cpp

}

DEFINES +=  ND_DEBUG _GUI_TOOL_

INCLUDEPATH += $$ndsdk_dir/include \
        $$game_dir/include \
        $$game_dir/cli_common \
        $$game_dir/logic_parser \
        $$game_dir/netMessage \
        $$game_dir/attribute

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
    ../../cli_common/gameMessage.cpp		\
    ../../cli_common/netui_atl.cpp \
    ../../cli_common/dftCliMsgHandler.cpp \
    ../../cli_common/login_apollo.cpp

HEADERS  += ../../cli_common/dftCliMsgHandler.h \
    ../../cli_common/login_apollo.h \
    ../../cli_common/apollo_robort.h	\
    ../../cli_common/gameMessage.h\
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
