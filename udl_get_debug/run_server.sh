#!/bin/bash

SCRIPT=`readlink -f $0`
SCRIPTPATH=`dirname $SCRIPT`
CURDIR=`pwd`
cd $SCRIPTPATH

PIPEFILE=cascade.pipe

cd cfg/server
rm -rf .plog
rm -f $PIPEFILE

mkfifo $PIPEFILE
tail -f $PIPEFILE | cascade_server &> $SCRIPTPATH/server.output &

sleep 1

cd $CURDIR

