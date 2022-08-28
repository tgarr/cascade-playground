#!/bin/bash

SCRIPT=`readlink -f $0`
SCRIPTPATH=`dirname $SCRIPT`
CURDIR=`pwd`
cd $SCRIPTPATH/cfg

CONFIG_TMP=derecho.cfg.rdma
DFGS_TMP=dfgs.json.tmp
DLL_TMP=udl_dlls.cfg.tmp
LAYOUT_TMP=layout.json.tmp

LAYOUT_CFG=layout.json
DFGS_CFG=dfgs.json
DLL_CFG=udl_dlls.cfg
DERECHO_CFG=derecho.cfg

mkdir -p server
ln -sf ../$LAYOUT_TMP server/$LAYOUT_CFG
ln -sf ../$DFGS_TMP server/$DFGS_CFG
ln -sf ../$DLL_TMP server/$DLL_CFG
cp $CONFIG_TMP server/$DERECHO_CFG

# read base values from derecho config
gms_port=`grep "^gms_port =" $CONFIG_TMP | cut -d "=" -f2 | tr -d " "`
state_transfer_port=`grep "^state_transfer_port =" $CONFIG_TMP | cut -d "=" -f2 | tr -d " "`
sst_port=`grep "^sst_port =" $CONFIG_TMP | cut -d "=" -f2 | tr -d " "`
rdmc_port=`grep "^rdmc_port =" $CONFIG_TMP | cut -d "=" -f2 | tr -d " "`
external_port=`grep "^external_port =" $CONFIG_TMP | cut -d "=" -f2 | tr -d " "`

let gms_port++
let state_transfer_port++
let sst_port++
let rdmc_port++
let external_port++

mkdir -p client
ln -sf ../$LAYOUT_TMP client/$LAYOUT_CFG
ln -sf ../$DFGS_TMP client/$DFGS_CFG
ln -sf ../$DLL_TMP client/$DLL_CFG

sed "s@^local_id = .*@local_id = 1@g" $CONFIG_TMP |
    sed "s@^gms_port = .*@gms_port = $gms_port@g" |
    sed "s@^state_transfer_port = .*@state_transfer_port = $state_transfer_port@g" |
    sed "s@^sst_port = .*@sst_port = $sst_port@g" |
    sed "s@^rdmc_port = .*@rdmc_port = $rdmc_port@g" |
    sed "s@^external_port = .*@external_port = $external_port@g" > client/$DERECHO_CFG

cd $CURDIR

