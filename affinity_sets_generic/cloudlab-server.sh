#!/bin/bash

SCRIPT=`readlink -f $0`
SCRIPTPATH=`dirname $SCRIPT`
CURDIR=`pwd`
cd $SCRIPTPATH

NODE_ID=$((`hostname -I | cut -d" " -f2 | cut -d. -f4`-1))

cd ./cfg/n$NODE_ID
rm -rf .plog
cascade_server

cd $CURDIR

