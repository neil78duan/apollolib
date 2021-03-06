# A simple test for the minimal standard C++ library
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := ../../../ndsdk/src/nd_common/src/bintree.c \
../../../ndsdk/src/nd_common/src/callstack.c \
../../../ndsdk/src/nd_common/src/common.c \
../../../ndsdk/src/nd_common/src/nd_cmdline.c \
../../../ndsdk/src/nd_common/src/nd_handle.c \
../../../ndsdk/src/nd_common/src/nd_mempool.c \
../../../ndsdk/src/nd_common/src/nd_mutex.c \
../../../ndsdk/src/nd_common/src/nd_static_alloc.c \
../../../ndsdk/src/nd_common/src/nd_str.c \
../../../ndsdk/src/nd_common/src/nd_time.c \
../../../ndsdk/src/nd_common/src/nd_timer.c \
../../../ndsdk/src/nd_common/src/nd_trace.c \
../../../ndsdk/src/nd_common/src/nd_unix.c \
../../../ndsdk/src/nd_common/src/nd_win.c \
../../../ndsdk/src/nd_common/src/nd_xml.c \
../../../ndsdk/src/nd_common/src/nddir.c \
../../../ndsdk/src/nd_common/src/node_mgr.c \
../../../ndsdk/src/nd_common/src/recbuf.c \
../../../ndsdk/src/nd_common/src/nd_iconv.c \
../../../ndsdk/src/nd_common/src/nd_logger.c \
../../../ndsdk/src/nd_common/src/cJSON.c \
../../../ndsdk/src/nd_vm/src/nd_asm.c \
../../../ndsdk/src/nd_vm/src/nd_formula.c \
../../../ndsdk/src/nd_vm/src/nd_vm.c \
../../../ndsdk/src/nd_vm/src/nd_vmdbg.c \
../../../ndsdk/src/nd_net/src/ipraw.c \
../../../ndsdk/src/nd_net/src/nd_msgentry.c \
../../../ndsdk/src/nd_net/src/nd_net.c \
../../../ndsdk/src/nd_net/src/nd_netobj.c \
../../../ndsdk/src/nd_net/src/nd_netui.c \
../../../ndsdk/src/nd_net/src/nd_socket.c \
../../../ndsdk/src/nd_net/src/nd_tcp.c \
../../../ndsdk/src/nd_net/src/nd_udp.c \
../../../ndsdk/src/nd_net/src/net_srv.c \
../../../ndsdk/src/nd_net/src/proxy_cli.c \
../../../ndsdk/src/nd_net/src/udt_net.c \
../../../ndsdk/src/nd_net/src/udt_socket.c \
../../../ndsdk/src/nd_net/src/udt_srv.c \
../../../ndsdk/src/nd_crypt/rsa/desc.c \
../../../ndsdk/src/nd_crypt/rsa/digit.c \
../../../ndsdk/src/nd_crypt/rsa/md2c.c \
../../../ndsdk/src/nd_crypt/rsa/md5c.c \
../../../ndsdk/src/nd_crypt/rsa/nn.c \
../../../ndsdk/src/nd_crypt/rsa/prime.c \
../../../ndsdk/src/nd_crypt/rsa/r_dh.c \
../../../ndsdk/src/nd_crypt/rsa/r_encode.c \
../../../ndsdk/src/nd_crypt/rsa/r_enhanc.c \
../../../ndsdk/src/nd_crypt/rsa/r_keygen.c \
../../../ndsdk/src/nd_crypt/rsa/r_random.c \
../../../ndsdk/src/nd_crypt/rsa/r_stdlib.c \
../../../ndsdk/src/nd_crypt/rsa/rsa.c \
../../../ndsdk/src/nd_crypt/src/nd_cryptfile.c \
../../../ndsdk/src/nd_crypt/src/nd_pubkey.c \
../../../ndsdk/src/nd_crypt/src/ndcrypt.c \
../../../ndsdk/src/nd_crypt/src/ndrsa.c \
../../../ndsdk/src/nd_crypt/src/tea.c \
../../../ndsdk/src/ndclient/nd_exch_key.cpp \
../../../ndsdk/src/ndclient/nd_client.cpp \
../../../ndsdk/src/ndapplib/nd_msgpack.cpp \
../../../ndsdk/src/ndapplib/nd_object.cpp \
../../../ndsdk/src/ndapplib/nd_connector.cpp \
../../../ndsdk/src/ndapplib/nd_baseConn.cpp \
../../../ndsdk/src/ndapplib/nd_vartype.cpp \
../../../ndsdk/src/ndapplib/nd_protoStream.cpp \
../../cli_common/login_apollo.cpp \
../../cli_common/dftCliMsgHandler.cpp \
../../cli_common/apoClientObj.cpp	\
../../plugin_u3d/apoCliApi.cpp	\
../../plugin_u3d/apoFormula.cpp	\
../../plugin_u3d/apoAttrCalcHelper.cpp	\
../../../nodeflow/src/parser/logicParser.cpp	\
../../../nodeflow/src/parser/logic_function.cpp	\
../../../nodeflow/src/parser/logicStruct.cpp	\
../../../nodeflow/src/parser/objectBaseMgr.cpp	\
../../../nodeflow/src/parser/logicDataType.cpp	\
../../../nodeflow/src/parser/logic_compile.cpp	\
../../../nodeflow/src/parser/logicEngineRoot.cpp	\
../../../nodeflow/src/parser/logic_editor_helper.cpp \
../../../nodeflow/src/parser/logic_debugger.cpp \
../../../nodeflow/src/parser/pluginsMgr.cpp \
../../../nodeflow/src/parser/logicApi4c.cpp \
../../game_parser/dbl_mgr.cpp	\
../../game_parser/dbldata2netstream.cpp	\
../../game_parser/apoGameCommon.cpp	\

LOCAL_CFLAGS := -D__ND_ADNROID__ -DND_CLIENT_ONLY
LOCAL_CPPFLAGS := -D__ND_ANDROID__  -DWITHOUT_LOGIC_PARSER -DND_CLIENT_ONLY

ifeq ($(apo_debug),1)
	LOCAL_CFLAGS += --debug  -DND_DEBUG -DDEBUG
	LOCAL_CPPFLAGS += --debug -DND_DEBUG -DDEBUG
	LOCAL_MODULE := apollo_android_d
else
	LOCAL_MODULE := apollo_android
endif

LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../include \
	$(LOCAL_PATH)/../../../ndsdk/include \
	$(LOCAL_PATH)/../.. \
	$(LOCAL_PATH)/../../plugin_u3d \
	$(LOCAL_PATH)/../../../nodeflow/include

#include $(BUILD_STATIC_LIBRARY)
include $(BUILD_SHARED_LIBRARY)

