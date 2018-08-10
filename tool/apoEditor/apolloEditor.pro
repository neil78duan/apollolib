#-------------------------------------------------
#
# Project created by QtCreator 2016-01-13T14:13:07
#
#-------------------------------------------------

ndsdk_dir = ../../../ndsdk
apolib_dir = ../..
apoBP_dir = ../../../apolloBP

macx:{

    QMAKE_MAC_SDK = macosx10.13
    DEFINES += __ND_MAC__
    platform_name = darwin_x86_64

#    LIBS += -lndclient_darwin_x86_64_d -llogic_parser_d -lapoBluePrint_d -liconv

}
unix:!macx{
    message(BUILD LINUX!)
    DEFINES += __ND_LINUX__
    platform_name = linux_x86_64

    LIBS += -lndclient_linux_x86_64_d -llogic_parser_d -lapoBluePrint_d -liconv
}

win32{
    message(WIN32!)
    platform_name = Win64

    CONFIG(debug, debug|release) {
        message(BUILD win32 -debug)
        LIBS += -lndclient_s_d -llogic_parser_d -lapoBluePrint_d
        DEFINES +=  ND_DEBUG
    } else {
        message(BUILD win32 -release)
        LIBS += -lndclient_s -llogic_parser -lapoBluePrint
    }

    DEFINES += __ND_WIN__ DO_NOT_CONVERT_PRINT_TEXT
}

LIBS += -L$$ndsdk_dir/lib  -L$$ndsdk_dir/lib/$$platform_name -L$$apolib_dir/lib/$$platform_name

DEFINES += X86_64

INCLUDEPATH += $$ndsdk_dir/include \
        $$apolib_dir/include \
        $$apolib_dir  ./include ./src \
        $$apoBP_dir/include $$apoBP_dir/apoScript


#-------qt----------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ../../../editor/apolloEditor
TEMPLATE = app


SOURCES += src/main.cpp\
    src/startdialog.cpp \
    src/workdirdialog.cpp \
    src/connectdialog.cpp

HEADERS  += \
    src/startdialog.h \
    src/workdirdialog.h \
    src/connectdialog.h

FORMS    += \
    src/startdialog.ui \
    src/connectdialog.ui \
    src/workdirdialog.ui

#cli-common
SOURCES += \
    $$apolib_dir/cli_common/apollo_robort.cpp\
    $$apolib_dir/cli_common/dftCliMsgHandler.cpp \
    $$apolib_dir/cli_common/login_apollo.cpp

HEADERS  += $$apolib_dir/cli_common/dftCliMsgHandler.h \
    $$apolib_dir/cli_common/login_apollo.h \
    $$apolib_dir/cli_common/apollo_robort.h

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

RESOURCES +=
