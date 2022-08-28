#!/bin/bash

SCRIPT=`readlink -f $0`
SCRIPTPATH=`dirname $SCRIPT`
CURDIR=`pwd`
CLIENTBIN=$SCRIPTPATH/udl_get

cd $SCRIPTPATH/cfg/client
$CLIENTBIN $@ | tee $SCRIPTPATH/client.output

cd $CURDIR

