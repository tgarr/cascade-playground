
#include "benchmark_workload.hpp"

// helper function to shuffle a list: usually this is not recommended, but using a vector in the generate() method below incurs a much biger overhead due to the frequent erase() calls. Thus we need a list, but we also need to shuffle it for the randomness.
static void shuffle_list(std::list<wallet_id_t>& list, std::mt19937& rng){
    // copy list to a vector
    std::vector<wallet_id_t> temp(list.begin(), list.end());

    // shuffle the vector
    std::shuffle(temp.begin(), temp.end(), rng);

    // copy shuffled vector back into the list
    std::copy(temp.begin(), temp.end(), list.begin());
}

void CBDCBenchmarkWorkload::generate(){
    if(generated){
        return;
    }

    // reset
    wallets.clear();
    transfers.clear();
    expected_final_values.clear();

    // create wallets and coins
    coin_id_t cid = 0;
    for(wallet_id_t wid=0;wid<num_wallets;wid++){
        for(coin_id_t i=0;i<coins_per_wallet;i++){
            wallets[wid].push_back(std::make_pair(cid,coin_initial_value));
            expected_final_values[std::make_pair(wid,cid)] += coin_initial_value;
            cid++;
        }
    }

    // generate list of all wallets that are going to be used (with repetitions), and shuffle it. 
    std::list<wallet_id_t> wallet_list;
    std::mt19937 rng(random_seed);
    
    for(wallet_id_t wid=0;wid<num_wallets;wid++){
        for(wallet_id_t i=0;i<transfers_per_wallet;i++){
            wallet_list.push_back(wid);
        }
    }
    
    shuffle_list(wallet_list,rng);

    // generate random transfers
    uint64_t wallets_per_transfer = senders_per_transfer + receivers_per_transfer;
    coin_value_t transfer_value_per_wallet = transfer_value * receivers_per_transfer;
    auto next_wallet_it = wallet_list.begin();
    while(wallet_list.size() >= wallets_per_transfer){
        benchmark_transfer_t transfer;
        std::unordered_map<wallet_id_t,bool> picked;
        bool finish = false;
        
        // pick senders
        auto started = next_wallet_it;
        while(transfer.senders.size() < senders_per_transfer){
            // find the next valid wallet 
            wallet_id_t sender_id = *next_wallet_it;
            while(picked.count(sender_id) > 0){
                // circular increment
                next_wallet_it++;
                if(next_wallet_it == wallet_list.end()){
                    next_wallet_it = wallet_list.begin();
                }

                if(next_wallet_it == started){
                    finish = true;
                    break;
                }

                sender_id = *next_wallet_it;
            }

            if(finish){
                break;
            }

            picked[sender_id] = true;
           
            // find the first valid coin for the transfer
            // this assumes only one coin is being used
            bool found = false;
            for(auto coin : wallets[sender_id]){
                auto coin_id = coin.first;
                auto item = std::make_pair(sender_id,coin_id);
                if(expected_final_values[item] >= transfer_value_per_wallet){
                    auto send_pair = std::make_pair(sender_id,std::vector<std::pair<coin_id_t,coin_value_t>>({std::make_pair(coin_id,transfer_value_per_wallet)}));
                    transfer.senders.push_back(send_pair);
                    expected_final_values[item] -= transfer_value_per_wallet;
                    found = true;
                    break;
                }
            }

            if(!found){
                throw std::runtime_error("Not enough funds. Try increasing the 'coin_initial_value' parameter.");
            }

            next_wallet_it = wallet_list.erase(next_wallet_it);
            if(next_wallet_it == wallet_list.end()){
                next_wallet_it = wallet_list.begin();
            }
        }

        if(finish){
            break;
        }

        // pick receivers
        while(transfer.receivers.size() < receivers_per_transfer){
            // find the next valid wallet 
            wallet_id_t receiver_id = *next_wallet_it;
            while(picked.count(receiver_id) > 0){
                // circular increment
                next_wallet_it++;
                if(next_wallet_it == wallet_list.end()){
                    next_wallet_it = wallet_list.begin();
                }

                if(next_wallet_it == started){
                    finish = true;
                    break;
                }

                receiver_id = *next_wallet_it;
            }

            if(finish){
                break;
            }

            picked[receiver_id] = true;

            std::vector<std::pair<coin_id_t,coin_value_t>> recv_pairs; 
            for(auto& input : transfer.senders){
                for(auto& send_pair : input.second){
                    coin_id_t coin_id = send_pair.first;
                    coin_value_t value_in = send_pair.second / receivers_per_transfer;

                    recv_pairs.push_back(std::make_pair(coin_id,value_in));
                    auto item = std::make_pair(receiver_id,coin_id);
                    expected_final_values[item] += value_in;
                }
            } 
            transfer.receivers.push_back(std::make_pair(receiver_id,recv_pairs));
            
            next_wallet_it = wallet_list.erase(next_wallet_it);
            if(next_wallet_it == wallet_list.end()){
                next_wallet_it = wallet_list.begin();
            }
        }

        if(!finish){
            transfers.push_back(transfer);
        }
    }

    generated = true;
}

void CBDCBenchmarkWorkload::to_file(std::string fname){
    if(!generated){
        generate();
    }

    ogzstream fout(fname.c_str());

    // parameters
    fout << num_wallets << " " << coins_per_wallet << " " << transfers_per_wallet << " " << senders_per_transfer << " " << receivers_per_transfer << " " << coin_initial_value << " " << transfer_value << " " << random_seed << " " << transfers.size() << " " << expected_final_values.size() << std::endl; 

    // wallets and coins
    for(auto& item : wallets){
        wallet_id_t wallet_id = item.first;
        for(auto& coin : item.second){
            fout << wallet_id << " " << coin.first << " " << coin.second << std::endl;
        }
    }

    // transfers
    for(auto& transfer : transfers){
        // senders
        for(auto& send_pair : transfer.senders){
            fout << send_pair.first << " " << send_pair.second.size() << " ";
            for(auto& coin_pair : send_pair.second){
                fout << coin_pair.first << " " << coin_pair.second << " ";
            }
        }

        // receivers
        for(auto& recv_pair : transfer.receivers){
            fout << recv_pair.first << " " << recv_pair.second.size() << " ";
            for(auto& coin_pair : recv_pair.second){
                fout << coin_pair.first << " " << coin_pair.second << " ";
            }
        }

        fout << std::endl;
    }

    // expected_final_values
    for(auto& item : expected_final_values){
        fout << item.first.first << " " << item.first.second << " " << item.second << std::endl;
    }

    fout.close();
}

CBDCBenchmarkWorkload& CBDCBenchmarkWorkload::from_file(std::string fname){
    igzstream fin(fname.c_str());

    wallet_id_t num_wallets;
    coin_id_t coins_per_wallet;
    uint64_t transfers_per_wallet;
    uint64_t senders_per_transfer;
    uint64_t receivers_per_transfer;
    coin_value_t coin_initial_value;
    coin_value_t transfer_value;
    uint64_t random_seed;
    uint64_t transfer_count;
    uint64_t expected_count;
    
    // read parameters
    fin >> num_wallets >> coins_per_wallet >> transfers_per_wallet >> senders_per_transfer >> receivers_per_transfer >> coin_initial_value >> transfer_value >> random_seed >> transfer_count >> expected_count;

    CBDCBenchmarkWorkload* benchmark = new CBDCBenchmarkWorkload(num_wallets,coins_per_wallet,transfers_per_wallet,senders_per_transfer,receivers_per_transfer,coin_initial_value,transfer_value,random_seed);

    // fill benchmark->wallets
    for(wallet_id_t i=0;i<num_wallets;i++){
        for(coin_id_t j=0;j<coins_per_wallet;j++){
            wallet_id_t wid;
            coin_id_t cid;
            coin_value_t cv;

            fin >> wid >> cid >> cv;
            benchmark->wallets[wid].push_back(std::make_pair(cid,cv));
        }
    }

    // fill benchmark->transfers
    for(uint64_t i=0;i<transfer_count;i++){
        benchmark_transfer_t transfer;

        // senders
        for(uint64_t j=0;j<senders_per_transfer;j++){
            wallet_id_t sender_id;
            uint64_t coin_count;
            fin >> sender_id >> coin_count;
            std::vector<std::pair<coin_id_t,coin_value_t>> coin_list;
            for(uint64_t k=0;k<coin_count;k++){
                coin_id_t cid;
                coin_value_t cv;
                fin >> cid >> cv;
                coin_list.push_back(std::make_pair(cid,cv));
            }

            transfer.senders.push_back(std::make_pair(sender_id,coin_list));
        }

        // receivers
        for(uint64_t j=0;j<receivers_per_transfer;j++){
            wallet_id_t recv_id;
            uint64_t coin_count;
            fin >> recv_id >> coin_count;
            std::vector<std::pair<coin_id_t,coin_value_t>> coin_list;
            for(uint64_t k=0;k<coin_count;k++){
                coin_id_t cid;
                coin_value_t cv;
                fin >> cid >> cv;
                coin_list.push_back(std::make_pair(cid,cv));
            }

            transfer.receivers.push_back(std::make_pair(recv_id,coin_list));
        }

        benchmark->transfers.push_back(transfer);
    }
    
    // fill benchmark->expected_final_values
    for(uint64_t i=0;i<expected_count;i++){
        wallet_id_t wid;
        coin_id_t cid;
        coin_value_t cv;

        fin >> wid >> cid >> cv;
        benchmark->expected_final_values[std::make_pair(wid,cid)] = cv;
    }

    benchmark->generated = true;
    return *benchmark;
}

