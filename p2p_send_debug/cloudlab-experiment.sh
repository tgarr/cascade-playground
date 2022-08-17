#!/bin/bash

SCRIPT=`readlink -f $0`
SCRIPTPATH=`dirname $SCRIPT`
CURDIR=`pwd`
CLIENTBIN=$SCRIPTPATH/debug_experiment

cd $SCRIPTPATH/cfg/client
rm -rf .plog
$CLIENTBIN $@ 2>> $SCRIPTPATH/local.results | tee $SCRIPTPATH/local.output

cd $CURDIR

