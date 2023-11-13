#!/bin/bash

NUM_NODES=$1
DERECHO_CFG=derecho.cfg
LAYOUT_CFG=layout.json

# read base values from derecho config
gms_port=`grep "^gms_port =" $DERECHO_CFG | cut -d "=" -f2 | tr -d " "`
state_transfer_port=`grep "^state_transfer_port =" $DERECHO_CFG | cut -d "=" -f2 | tr -d " "`
sst_port=`grep "^sst_port =" $DERECHO_CFG | cut -d "=" -f2 | tr -d " "`
rdmc_port=`grep "^rdmc_port =" $DERECHO_CFG | cut -d "=" -f2 | tr -d " "`
external_port=`grep "^external_port =" $DERECHO_CFG | cut -d "=" -f2 | tr -d " "`

i=0
while (( i < NUM_NODES )); do
    # cascade configs
    mkdir -p n$i
    ln -sf ../$LAYOUT_CFG n$i/$LAYOUT_CFG

    # derecho config
    sed "s@^local_id = .*@local_id = $i@g" $DERECHO_CFG |
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
    let i++
done

