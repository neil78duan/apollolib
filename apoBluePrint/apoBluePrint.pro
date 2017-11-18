#-------------------------------------------------
#
# Project created by QtCreator 2017-11-18T15:15:42
#
#-------------------------------------------------


ndsdk_dir = ../../ndsdk
apolib_dir = ..

macx:{

QMAKE_MAC_SDK = macosx10.13
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
LIBS += -L$$ndsdk_dir/lib/win64

CONFIG(debug, debug|release) {
    message(BUILD win32 -debug)
    LIBS += -lndclient_s_d
    DEFINES +=  ND_DEBUG
} else {
    message(BUILD win32 -release)
    LIBS += -lndclient_s
}

DEFINES += __ND_WIN__ DO_NOT_CONVERT_PRINT_TEXT
}


DEFINES += X86_64

INCLUDEPATH += $$ndsdk_dir/include \
        $$apolib_dir/include \
        $$apolib_dir  ./include ./include/apoScript


#----------------------------------------
QT       += widgets

TARGET = apoBluePrint
TEMPLATE = lib
CONFIG += staticlib

unix {
    target.path = /usr/lib
    INSTALLS += target
}

SOURCES += apoblueprint.cpp \
    apoScript/xmldialog.cpp \
    apoScript/listdialog.cpp \
    apoScript/dragtree.cpp \
    apoScript/apoBaseExeNode.cpp \
    apoScript/apoUiBezier.cpp \
    apoScript/apoUiExeNode.cpp \
    apoScript/apoUiMainEditor.cpp \
    apoScript/mainwindow.cpp \
    apoScript/editorframe.cpp \
    apoScript/apoXmlTreeView.cpp \
    apoScript/apoUiDetailView.cpp \
    apoScript/apoUiParam.cpp \
    apoScript/apoUiXmlTablesWidget.cpp \
    apoScript/apoEditorSetting.cpp \
    apoScript/runFuncDialog.cpp \
    apoScript/apoUiDebugger.cpp

HEADERS += apoblueprint.h\
    include/apoScript/xmldialog.h \
    include/apoScript/listdialog.h \
    include/apoScript/dragtree.h \
    include/apoScript/myqtitemctrl.h \
    include/apoScript/apoBaseExeNode.h \
    include/apoScript/apoUiBezier.h \
    include/apoScript/apoUiExeNode.h \
    include/apoScript/apoUiMainEditor.h \
    include/apoScript/apouinodedef.h \
    include/apoScript/mainwindow.h \
    include/apoScript/editorFrame.h \
    include/apoScript/apoXmlTreeView.h \
    include/apoScript/apoUiDetailView.h \
    include/apoScript/apoUiParam.h \
    include/apoScript/apoUiXmlTablesWidget.h \
    include/apoScript/runFuncDialog.h \
    include/apoScript/apoUiDebugger.h

FORMS    += \
    apoScript/listdialog.ui \
    apoScript/xmldialog.ui \
    apoScript/mainwindow.ui \
    apoScript/mainwindow.ui \
    apoScript/runFuncDialog.ui
