#!/bin/bash

# create pools
for i in `seq 0 99`; do 
    cascade_client create_object_pool /test_$i VCSS 0
done


