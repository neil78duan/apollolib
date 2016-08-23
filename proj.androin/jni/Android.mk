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
../../../ndsdk/src/nd_common/src/source_log.c \
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
../../../ndsdk/src/nd_net/src/udt_icmp.c \
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
../../../ndsdk/src/ndclient/c_api/msg_format.cpp \
../../../ndsdk/src/ndclient/c_api/nd_connector.c \
../../../ndsdk/src/ndclient/c_api/nd_exch_key.cpp \
../../../ndsdk/src/ndclient/cpp_api/nd_client.cpp \
../../../ndsdk/src/ndapplib/nd_msgpack.cpp \
../../../ndsdk/src/ndapplib/nd_datatransfer.cpp \
../../cli_common/login_apollo.cpp \
../../cli_common/netui_atl.cpp	\
../../logic_parser/dbl_mgr.cpp	\
../../logic_parser/logicParser.cpp	\
../../logic_parser/logic_function.cpp	\
../../logic_parser/dbldata2netstream.cpp	\
../../logic_parser/logicStruct.cpp	\
../../logic_parser/objectBaseMgr.cpp	\
../../logic_parser/logicDataType.cpp	\
../../logic_parser/logic_compile.cpp	\
../../logic_parser/logicEngineRoot.cpp	\
../../logic_parser/logic_editor_helper.cpp

LOCAL_CFLAGS := -D__ND_ADNROID__
LOCAL_CPPFLAGS := -D__ND_ANDROID__

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
	$(LOCAL_PATH)/../..

include $(BUILD_STATIC_LIBRARY)
