#!/bin/bash

object_size="1000"
object_rate="50 100 150 200 250 300"
data_part_size="1000000 10000000 100000000"
num_data_parts="10 20 30 40 50 60 70 80 90 100"

for dpsz in $data_part_size; do
    for ndp in $num_data_parts; do
        for osz in $object_size; do
            for orate in $object_rate; do
                ./cloudlab-client.sh $osz $orate $dpsz $ndp
            done
        done
    done
done
