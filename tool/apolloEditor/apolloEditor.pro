#-------------------------------------------------
#
# Project created by QtCreator 2016-01-13T14:13:07
#
#-------------------------------------------------

ndsdk_dir = ../../server/ndsdk
game_dir = ../..//server

macx:{
message(BUILD MACOX)
DEFINES += __MAC_OS__ ND_UNIX

LIBS += -L$$ndsdk_dir/lib -lndclient_darwin_x86_64_d \
    -L$$game_dir/lib/darwin_x86_64  -lnetMessage_d  \
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
    ../../server/src/netMessage/dataStream.cpp\
    ../../server/src/netMessage/MessageName.cpp\
    ../../server/src/netMessage/auto_dataType.cpp\
    ../../server/src/netMessage/dbDataConvert.cpp

}

DEFINES +=  ND_DEBUG _GUI_TOOL_

INCLUDEPATH += $$ndsdk_dir/include \
        $$game_dir/include \
        $$game_dir/src/cli_common \
        $$game_dir/src/logic_compile \
        $$game_dir/src/netMessage \
        $$game_dir/src/attribute

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
    ../../server/src/logic_compile/logicDataType.cpp \
    ../../server/src/logic_compile/logicEngineRoot.cpp \
    ../../server/src/logic_compile/logicParser.cpp \
    ../../server/src/logic_compile/logic_compile.cpp \
    ../../server/src/logic_compile/logic_editor_helper.cpp \
    ../../server/src/logic_compile/objectBaseMgr.cpp \
    ../../server/src/logic_compile/dbl_mgr.cpp \
    ../../server/src/logic_compile/logic_function.cpp \
    ../../server/src/logic_compile/dbldata2netstream.cpp \
    ../../server/src/logic_compile/logicStruct.cpp

HEADERS  += ../../server/src/logic_compile/dbl_mgr.h \
    ../../server/src/logic_compile/logicDataType.h \
    ../../server/src/logic_compile/logicEngineRoot.h \
    ../../server/src/logic_compile/logicParser.h \
    ../../server/src/logic_compile/logic_compile.h \
    ../../server/src/logic_compile/logic_editor_helper.h \
    ../../server/src/logic_compile/objectBaseMgr.h \
    ../../server/src/logic_compile/logic_function.h \
    ../../server/src/logic_compile/dbldata2netstream.h \
    ../../server/src/logic_compile/logicStruct.hpp

#cli-common
SOURCES += \
    ../../server/src/cli_common/apollo_robort.cpp\	
    ../../server/src/cli_common/gameMessage.cpp		\
    ../../server/src/cli_common/netui_atl.cpp \
    ../../server/src/cli_common/dftCliMsgHandler.cpp \	
    ../../server/src/cli_common/login_apollo.cpp 

HEADERS  += ../../server/src/cli_common/dftCliMsgHandler.h \	
    ../../server/src/cli_common/login_apollo.h \
    ../../server/src/cli_common/apollo_robort.h	\
    ../../server/src/cli_common/gameMessage.h\		
    ../../server/src/cli_common/netui_atl.h 

# attribute 

SOURCES += \
    ../../server/src/attribute/roleattr_help.cpp	\
    ../../server/src/attribute/stat_machine.cpp \
    ../../server/src/attribute/attr_mgr.cpp	   \
    ../../server/src/attribute/stat_data.cpp
    
HEADERS  += ../../server/src/attribute/stat_data.h \
    ../../server/src/attribute/stat_machine.h \
    ../../server/src/attribute/attr_mgr.h \
    ../../server/src/attribute/roleattr_help.h	
