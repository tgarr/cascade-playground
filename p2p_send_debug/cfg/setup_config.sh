#!/bin/bash

NUM_SHARDS=2

SCRIPT=`readlink -f $0`
SCRIPTPATH=`dirname $SCRIPT`
CURDIR=`pwd`
cd $SCRIPTPATH

RDMA_TMP=derecho.cfg.rdma
DFGS_TMP=dfgs.json.tmp
DLL_TMP=udl_dlls.cfg.tmp
LAYOUT_TMP=layout.json.tmp

LAYOUT_CFG=layout.json
DFGS_CFG=dfgs.json
DLL_CFG=udl_dlls.cfg
DERECHO_CFG=derecho.cfg

i=0
while (( i <= NUM_SHARDS + 1 )); do
    # cascade configs
    mkdir -p n$i
    ln -sf ../$LAYOUT_TMP n$i/$LAYOUT_CFG
    #ln -sf ../$DFGS_TMP n$i/$DFGS_CFG
    #ln -sf ../$DLL_TMP n$i/$DLL_CFG

    # derecho config
    node_ip=$((i+1))
    sed "s@^local_id = .*@local_id = $i@g" $RDMA_TMP | sed "s@XXX_LOCAL_IP_XXX@$node_ip@g" > n$i/$DERECHO_CFG

    let i++
done

# client config
rm -rf client
let i--
mv n$i client

cd $CURDIR

