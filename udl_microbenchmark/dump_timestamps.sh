#!/bin/bash

if (( $# < 2 )); then
    echo "usage: $0 <local|remote> <dump_file_name>"
    exit
fi

shard=
if [[ "$1" == "local" ]]; then
    shard=0
elif [[ "$1" == "remote" ]]; then
    shard=1
fi

SCRIPT=`readlink -f $0`
SCRIPTPATH=`dirname $SCRIPT`
CURDIR=`pwd`

if ! [ -z $shard ]; then
    cd $SCRIPTPATH/cfg/client
    cascade_client dump_timestamp VCSS 0 $shard $2

    cd $CURDIR
fi

