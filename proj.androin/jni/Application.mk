APP_STL := c++_static

APP_PLATFORM=android-16

#APP_ABI := arm64-v8a  armeabi-v7a x86
APP_ABI := armeabi-v7a

APP_CPPFLAGS := -frtti -D__ND_ANDROID__  -DND_CLIENT_ONLY
APP_CFLAGS :=  -D__ND_ANDROID__ -DND_CLIENT_ONLY

ifeq ($(apo_debug),1)
APP_OPTIM := debug
else
APP_OPTIM := release
endif

