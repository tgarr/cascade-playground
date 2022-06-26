#!/bin/bash

SCRIPT=`readlink -f $0`
SCRIPTPATH=`dirname $SCRIPT`
CURDIR=`pwd`
CLIENTBIN=$SCRIPTPATH/pipeline_client

cd $SCRIPTPATH/cfg/client
$CLIENTBIN $@ 2>> $SCRIPTPATH/client.results | tee $SCRIPTPATH/client.output

cd $CURDIR

