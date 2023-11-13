#!/bin/bash

set -x

# create pools
cascade_client create_object_pool /test VCSS 0

# put object
cascade_client op_put /test/test 0

