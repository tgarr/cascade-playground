#!/bin/bash

object_size="1000"
object_rate="10 20 30 40 50"
entry_part_size="1000000"
num_entry_parts="1"
data_part_size="1000000"
num_data_parts="10 20 30 40 50"

for epsz in $entry_part_size; do
    for nep in $num_entry_parts; do
        for dpsz in $data_part_size; do
            for ndp in $num_data_parts; do
                for osz in $object_size; do
                    for orate in $object_rate; do
                        if ! grep -q " $osz $orate $epsz $nep $dpsz $ndp " client.results; then
                            ./cloudlab-client.sh $osz $orate $epsz $nep $dpsz $ndp
                            sleep 2
                        fi
                    done
                done
            done
        done
    done
done

