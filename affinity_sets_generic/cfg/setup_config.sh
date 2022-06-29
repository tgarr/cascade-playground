#!/bin/bash

NUM_SHARDS=2
MODE="local"

if (( $# > 0 )); then
    NUM_SHARDS=$1
    if (( NUM_SHARDS < 1 )); then
        NUM_SHARDS=2
    fi

    if [[ "$2" == "rdma" ]]; then
        MODE="rdma"
    fi
fi

SCRIPT=`readlink -f $0`
SCRIPTPATH=`dirname $SCRIPT`
CURDIR=`pwd`
cd $SCRIPTPATH

LOCAL_TMP=derecho.cfg.local
RDMA_TMP=derecho.cfg.rdma
DFGS_TMP=dfgs.json.tmp
DLL_TMP=udl_dlls.cfg.tmp
LAYOUT_TMP=layout.json.tmp

LAYOUT_CFG=layout.json
DFGS_CFG=dfgs.json
DLL_CFG=udl_dlls.cfg
DERECHO_CFG=derecho.cfg

# read base values from derecho config
gms_port=`grep "^gms_port =" $LOCAL_TMP | cut -d "=" -f2 | tr -d " "`
state_transfer_port=`grep "^state_transfer_port =" $LOCAL_TMP | cut -d "=" -f2 | tr -d " "`
sst_port=`grep "^sst_port =" $LOCAL_TMP | cut -d "=" -f2 | tr -d " "`
rdmc_port=`grep "^rdmc_port =" $LOCAL_TMP | cut -d "=" -f2 | tr -d " "`
external_port=`grep "^external_port =" $LOCAL_TMP | cut -d "=" -f2 | tr -d " "`

num_nodes="\"1\""
delivery="\"Ordered\""
reserved="[\"1\"]"
profiles="\"EVAL\""
i=0
while (( i <= NUM_SHARDS + 1 )); do
    # layout
    if (( i > 1 & i <= NUM_SHARDS )); then
        num_nodes="$num_nodes,\"1\""
        delivery="$delivery,\"Ordered\""
        reserved="$reserved,[\"$i\"]"
        profiles="$profiles,\"EVAL\""
    fi

    # cascade configs
    mkdir -p n$i
    ln -sf ../$LAYOUT_CFG n$i/$LAYOUT_CFG
    ln -sf ../$DFGS_TMP n$i/$DFGS_CFG
    ln -sf ../$DLL_TMP n$i/$DLL_CFG

    # derecho config
    if [[ "$MODE" == "local" ]]; then
        sed "s@^local_id = .*@local_id = $i@g" $LOCAL_TMP |
        sed "s@^gms_port = .*@gms_port = $gms_port@g" |
        sed "s@^state_transfer_port = .*@state_transfer_port = $state_transfer_port@g" |
        sed "s@^sst_port = .*@sst_port = $sst_port@g" |
        sed "s@^rdmc_port = .*@rdmc_port = $rdmc_port@g" |
        sed "s@^external_port = .*@external_port = $external_port@g" > n$i/$DERECHO_CFG

        let gms_port++
        let state_transfer_port++
        let sst_port++
        let rdmc_port++
        let external_port++
    else
        node_ip=$((i+1))
        sed "s@^local_id = .*@local_id = $i@g" $RDMA_TMP | sed "s@XXX_LOCAL_IP_XXX@$node_ip@g" > n$i/$DERECHO_CFG
    fi

    let i++
done

# client config
rm -rf client
let i--
mv n$i client

# layout
sed "s@XXX_MIN_NODES_XXX@$num_nodes@g" $LAYOUT_TMP | 
    sed "s@XXX_MAX_NODES_XXX@$num_nodes@g" |
    sed "s@XXX_DELIVERY_MODES_XXX@$delivery@g" |
    sed "s@XXX_RESERVED_IDS_XXX@$reserved@g" |
    sed "s@XXX_PROFILES_XXX@$profiles@g" > $LAYOUT_CFG

cd $CURDIR

