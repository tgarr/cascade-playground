#!/bin/bash

SCRIPT=`readlink -f $0`
SCRIPTPATH=`dirname $SCRIPT`
CURDIR=`pwd`
BASE_IP=29
cd $SCRIPTPATH

NODE_ID=$((`hostname -I | cut -d" " -f2 | cut -d. -f4`-BASE_IP))

cd ./cfg/n$NODE_ID
rm -rf .plog
cascade_server

cd $CURDIR

