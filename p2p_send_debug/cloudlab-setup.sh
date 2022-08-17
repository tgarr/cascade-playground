#!/bin/bash

SCRIPT=`readlink -f $0`
SCRIPTPATH=`dirname $SCRIPT`
CURDIR=`pwd`
CLIENTBIN=$SCRIPTPATH/debug_setup

cd $SCRIPTPATH/cfg/client
rm -rf .plog
$CLIENTBIN

cd $CURDIR

