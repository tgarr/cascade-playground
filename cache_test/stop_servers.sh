#!/bin/bash

SCRIPT=`readlink -f $0`
SCRIPTPATH=`dirname $SCRIPT`
CURDIR=`pwd`
cd $SCRIPTPATH

echo stop > cfg/n0/cascade.pipe
echo stop > cfg/n1/cascade.pipe
sleep 1

cd $CURDIR

