#!/bin/bash

SCRIPT=`readlink -f $0`
SCRIPTPATH=`dirname $SCRIPT`
CURDIR=`pwd`
cd $SCRIPTPATH

PIPEFILE=cascade.pipe
SHOWLINES=40

num=0
for i in ./cfg/n*; do
    cd $i
    rm -rf .plog
    rm -f $PIPEFILE

    echo "Starting `basename $i` ..."
    mkfifo $PIPEFILE
    tail -f $PIPEFILE | cascade_server &> $SCRIPTPATH/`basename $i`.output &
    sleep 1

    cd - &> /dev/null
    let num++
done

watch -n 0.5 tail -n $((SHOWLINES/num)) n*.output

for i in ./cfg/n*; do
    echo stop > $i/$PIPEFILE
done

sleep 1

cd $CURDIR

