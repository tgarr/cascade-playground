#!/bin/bash

if [ -z $1 ]; then
    echo "usage: $0 <dump_file_name>"
    exit
fi

SCRIPT=`readlink -f $0`
SCRIPTPATH=`dirname $SCRIPT`
CURDIR=`pwd`

cd $SCRIPTPATH/cfg/client
cascade_client dump_timestamp VCSS 0 0 $1

mv $SCRIPTPATH/cfg/n1/$1 $SCRIPTPATH
cd $CURDIR

