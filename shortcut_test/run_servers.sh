#!/bin/bash

SCRIPT=`readlink -f $0`
SCRIPTPATH=`dirname $SCRIPT`
CURDIR=`pwd`
PIPEFILE=cascade.pipe

# metadata service
cd $SCRIPTPATH/cfg/n0
rm -rf .plog
rm -f $PIPEFILE

mkfifo $PIPEFILE
tail -f $PIPEFILE | cascade_server &> $SCRIPTPATH/n0.output &
sleep 1

# cascade k/v store
cd $SCRIPTPATH/cfg/n1
rm -rf .plog
rm -f $PIPEFILE

mkfifo $PIPEFILE
tail -f $PIPEFILE | cascade_server &> $SCRIPTPATH/n1.output &
sleep 1

cd $CURDIR

