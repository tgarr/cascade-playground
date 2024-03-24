# Cascade CBDC
Implementing a CBDC on top of Cascade. This makes use of the new atomic multi-object put in Cascade.

## Requirements
The following is necessary for compiling this project:
- Cascade (https://github.com/Derecho-Project/cascade)
- gzstream and zlib (in Ubuntu: `apt install libgzstream-dev zlib1g-dev`)

## Generating benchmark workload
The `generate_workload` executable generates a workload that can be used to run a benchmark. The workload is saved in a zipped file.
```
$ ./generate_workload -h
usage: ./generate_workload [options]
options:
 -w <num_wallets>               number of wallets to create
 -c <coins_per_wallet>          number of coins to create in each wallet
 -t <transfers_per_wallet>      how many times each wallet appears across all transfers
 -s <senders_per_transfer>      number of wallets sending coins in each transfer
 -r <receivers_per_transfer>    number of wallets receiving coins in each transfer
 -i <coin_initial_value>        initial value of each minted coin
 -v <transfer_value>            amount transfered to each receiver
 -g <random_seed>               seed for the RNG
 -o <output_file>               file to write the generated workload (zipped)
 -h                             show this help
```

The default name of the output file is a aggregation of all parameters, with the extension `.gz`. The generated file can be seen uzing `zcat`. For example:
```
$ ./generate_workload -w 4
parameters:
 num_wallets = 4
 coins_per_wallet = 1
 transfers_per_wallet = 1
 senders_per_transfer = 1
 receivers_per_transfer = 1
 coin_initial_value = 100000
 transfer_value = 10
 random_seed = 3
 output_file = 4_1_1_1_1_100000_10_3.gz

generating ...
writing to '4_1_1_1_1_100000_10_3.gz' ...
done
$ zcat 4_1_1_1_1_100000_10_3.gz
4 1 1 1 1 100000 10 3 2 6
0 0 100000
1 1 100000
2 2 100000
3 3 100000
3 1 3 10 1 1 3 10 
0 1 0 10 2 1 0 10 
0 0 99990
1 1 100000
1 3 10
2 0 10
2 2 100000
3 3 99990
```

The generated file can be loaded using the static method `CBDCBenchmarkWorkload::from_file(filename)`. The workload provides three structures that can be used to execute a benchmark:
- Wallets/coins to be minted: obtained by calling `get_wallets()`
    - This is a map from `wallet_id_t` to a vector of pairs (coin\_id\_t,coin\_value\_t). Each pair is a coin to be created in the corresponding wallet.
- Transfers: obtained by calling `get_transfers()`
    - This is a vector of transfer structures. Each transfer contains a vector of senders amd a vector of receivers. Each sender/receiver is a pair (wallet\_id\_t,vector of (coin\_id\_t,coin\_value\_t))
- Expected values: obtained by calling `get_expected_final_values()`
    - This is a map between a pair (wallet\_id\_t,coin\_id\_t) and coin\_value\_t, corresponding to the expected value of that coin after all transfers have been executed succesfully. This should be used to verify the correctness of the benchmark.

