#!/bin/bash

SCRIPT=`readlink -f $0`
SCRIPTPATH=`dirname $SCRIPT`
CURDIR=`pwd`
CLIENTBIN=$SCRIPTPATH/debug_experiment

cd $SCRIPTPATH/cfg/client
rm -rf .plog
$CLIENTBIN $@ | tee $SCRIPTPATH/experiment.output

cd $CURDIR

