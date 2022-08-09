#!/bin/bash

SCRIPT=`readlink -f $0`
SCRIPTPATH=`dirname $SCRIPT`
CURDIR=`pwd`
CLIENTBIN=$SCRIPTPATH/pipeline_client

NODE_IP=`hostname -I | cut -d" " -f2`

cd $SCRIPTPATH/cfg/client
rm -rf .plog
$CLIENTBIN $1 $2 $3 $4 $5 $6 $NODE_IP $7 2>> $SCRIPTPATH/client.results | tee $SCRIPTPATH/client.output

cd $CURDIR

