#!/bin/bash

SCRIPT=`readlink -f $0`
SCRIPTPATH=`dirname $SCRIPT`
CURDIR=`pwd`
CLIENTBIN=$SCRIPTPATH/pipeline_client

NODE_IP=`hostname -I | cut -d" " -f2`
sudo modprobe ib_uverbs

cd $SCRIPTPATH/cfg/client
rm -rf .plog
$CLIENTBIN $@ $NODE_IP 2>> $SCRIPTPATH/client.results | tee $SCRIPTPATH/client.output

cd $CURDIR

