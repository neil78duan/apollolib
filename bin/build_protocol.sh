#!/bin/sh

./bin/protoBuilder -d config -encode utf8
[ $? == 0 ] || exit 1

cp ./cpp/* ../../server/src/netMessage
