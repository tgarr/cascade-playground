#!/bin/bash

SCRIPT=`readlink -f $0`
SCRIPTPATH=`dirname $SCRIPT`
CURDIR=`pwd`
CLIENTBIN=$SCRIPTPATH/udl_microbenchmark

cd $SCRIPTPATH/cfg/client
rm -rf .plog
$CLIENTBIN $@ | tee $SCRIPTPATH/client.output

cd $CURDIR

