#!/bin/sh

#  config.sh
#  config for apollo library
#
#  Created by duan .
#

workDir=`pwd`


create_apohome() {
    cd $HOME
    if [ -f .bash_profile ]; then
        echo "export LIBAPOLLO=\"$workDir\"" >> .bash_profile
        source $HOME/.bash_profile
    elif [ -f .profile ]; then
        echo "export LIBAPOLLO=\"$workDir\"" >> .profile
        source $HOME/.profile
    else
        echo "could not create LIBAPOLLO "
        echo "PLEASE set evn $LIBAPOLLO "
        cd $workDir
        exit 1
    fi
    cd $workDir
}

# create env

if [ "x$LIBAPOLLO" == "x" ]; then
    create_apohome
fi

# create output dir
PLATFORM_BITS=`getconf LONG_BIT`

[ -d bin ] || mkdir bin
[ -d lib ] || mkdir lib


ARCH_MACHINE=`uname -m`
OS_kernel=`uname -s |  tr '[A-Z]' '[a-z]'`

WORKDIR="./bin/"$OS_kernel"_"$ARCH_MACHINE
LIBDIR="./lib/"$OS_kernel"_"$ARCH_MACHINE

[ -d  $WORKDIR ] || mkdir $WORKDIR
[ -d  $LIBDIR ] || mkdir $LIBDIR

echo "config SUCCESS "


