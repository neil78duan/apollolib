#-------------------------------------------------
#
# Project created by QtCreator 2016-01-13T14:13:07
#
#-------------------------------------------------

ndsdk_dir = ../../../ndsdk
apolib_dir = ../..

macx:{
#apolib_dir = $$(LIBAPOLLO)
#ndsdk_dir = $$(NDHOME)


QMAKE_MAC_SDK = macosx10.12

DEFINES += __ND_MAC__

LIBS += -L$$ndsdk_dir/lib -L$$apolib_dir/lib/darwin_x86_64  -liconv -L$$ndsdk_dir/lib/darwin_x86_64

LIBS += -lnd_vm_dbg -lndclient_darwin_x86_64_d

}
unix:!macx{
message(BUILD LINUX!)
DEFINES += __ND_LINUX__
}

win32{
message(WIN32!)
#ndsdk_dir = ../../../ndsdk
#apolib_dir = ../..
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
        $$apolib_dir/include \
        $$apolib_dir

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
    newfiledialog.cpp \
    apoScrpit/apoBaseExeNode.cpp \
    apoScrpit/apoUiBezier.cpp \
    apoScrpit/apoUiExeNode.cpp \
    apoScrpit/apoUiMainEditor.cpp \
    apoScrpit/mainwindow.cpp \
    apoScrpit/editorframe.cpp \
    apoScrpit/apoXmlTreeView.cpp \
    apoScrpit/apoUiDetailView.cpp \
    apoScrpit/apoUiParam.cpp \
    apoScrpit/apoUiXmlTablesWidget.cpp \
    apoScrpit/apoEditorSetting.cpp

HEADERS  += \
    startdialog.h \
    xmldialog.h \
    connectdialog.h \
    listdialog.h \
    dragtree.h \
    workdirdialog.h \
    newfiledialog.h \
    myqtitemctrl.h \
    apoScrpit/apoBaseExeNode.h \
    apoScrpit/apoUiBezier.h \
    apoScrpit/apoUiExeNode.h \
    apoScrpit/apoUiMainEditor.h \
    apoScrpit/apouinodedef.h \
    apoScrpit/mainwindow.h \
    apoScrpit/editorFrame.h \
    apoScrpit/apoXmlTreeView.h \
    apoScrpit/apoUiDetailView.h \
    apoScrpit/apoUiParam.h \
    apoScrpit/apoUiXmlTablesWidget.h

FORMS    += \
    startdialog.ui \
    xmldialog.ui \
    connectdialog.ui \
    listdialog.ui \
    workdirdialog.ui \
    newfiledialog.ui \
    apoScrpit/mainwindow.ui \
    apoScrpit/mainwindow.ui

# logic compile modules
 
SOURCES += \
    $$apolib_dir/logic_parser/logicDataType.cpp \
    $$apolib_dir/logic_parser/logicEngineRoot.cpp \
    $$apolib_dir/logic_parser/logicParser.cpp \
    $$apolib_dir/logic_parser/logic_compile.cpp \
    $$apolib_dir/logic_parser/logic_editor_helper.cpp \
    $$apolib_dir/logic_parser/objectBaseMgr.cpp \
    $$apolib_dir/logic_parser/dbl_mgr.cpp \
    $$apolib_dir/logic_parser/logic_function.cpp \
    $$apolib_dir/logic_parser/dbldata2netstream.cpp \
    $$apolib_dir/logic_parser/logicStruct.cpp

HEADERS  += $$apolib_dir/logic_parser/dbl_mgr.h \
    $$apolib_dir/logic_parser/logicDataType.h \
    $$apolib_dir/logic_parser/logicEngineRoot.h \
    $$apolib_dir/logic_parser/logicParser.h \
    $$apolib_dir/logic_parser/logic_compile.h \
    $$apolib_dir/logic_parser/logic_editor_helper.h \
    $$apolib_dir/logic_parser/objectBaseMgr.h \
    $$apolib_dir/logic_parser/logic_function.h \
    $$apolib_dir/logic_parser/dbldata2netstream.h \
    $$apolib_dir/logic_parser/logicStruct.hpp

#cli-common
SOURCES += \
    $$apolib_dir/cli_common/apollo_robort.cpp\
    $$apolib_dir/cli_common/netui_atl.cpp \
    $$apolib_dir/cli_common/dftCliMsgHandler.cpp \
    $$apolib_dir/cli_common/login_apollo.cpp

HEADERS  += $$apolib_dir/cli_common/dftCliMsgHandler.h \
    $$apolib_dir/cli_common/login_apollo.h \
    $$apolib_dir/cli_common/apollo_robort.h	\
    $$apolib_dir/cli_common/netui_atl.h

# attribute 

SOURCES += \
    $$apolib_dir/attribute/roleattr_help.cpp	\
    $$apolib_dir/attribute/stat_machine.cpp \
    $$apolib_dir/attribute/attr_mgr.cpp	   \
    $$apolib_dir/attribute/stat_data.cpp
    
HEADERS  += $$apolib_dir/attribute/stat_data.h \
    $$apolib_dir/attribute/stat_machine.h \
    $$apolib_dir/attribute/attr_mgr.h \
    $$apolib_dir/attribute/roleattr_help.h
