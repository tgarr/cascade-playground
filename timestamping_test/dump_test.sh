#!/bin/bash

set -x

# create pools
cascade_client create_object_pool /volatile_0 VCSS 0 && sleep 2
cascade_client create_object_pool /volatile_1 VCSS 1 && sleep 2
cascade_client create_object_pool /persistent_0 PCSS 0 && sleep 2

# put objects
cascade_client op_put /volatile_0/test 0 && sleep 2
cascade_client op_put /volatile_1/test 0 && sleep 2
cascade_client op_put /persistent_0/test 0 && sleep 2

# dump timestamps
cascade_client op_dump_timestamp /volatile_0 volatile_0.dump && sleep 2
cascade_client op_dump_timestamp /volatile_1 volatile_1.dump && sleep 2
cascade_client op_dump_timestamp /persistent_0 persistent_0.dump && sleep 2

