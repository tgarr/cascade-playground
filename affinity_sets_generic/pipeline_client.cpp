#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <atomic>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <pthread.h>
#include "common.hpp"

bool usage(int argc, char** argv){
    if(argc >= 6) return true;
    std::cout << argv[0] << " <object_size> <object_rate> <data_part_size> <num_data_parts> <client_ip_address>" << std::endl;
    return false;
}

inline void cpu_affinity(int core_id){
    pthread_t thread = pthread_self();
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id,&cpuset);
    pthread_setaffinity_np(thread,sizeof(cpu_set_t),&cpuset);
}

void listen_notifications(std::chrono::high_resolution_clock::time_point* rcv_timestamps,std::atomic<bool>* stop_listen,node_id_t client_id,char* client_ip){
    char buffer[RETURN_MESSAGE_SIZE];
    struct sockaddr_in servaddr,cliaddr;
    
    cpu_affinity(6);
    
    // address structs
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
    unsigned int len = sizeof(cliaddr);

    // create socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(CLIENT_RETURN_UDP_PORT + client_id);
    servaddr.sin_addr.s_addr = inet_addr(client_ip);

    // timeout
    struct timeval tv;
    tv.tv_sec = CLIENT_RETURN_TIMEOUT;
    tv.tv_usec = 0;
    setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(tv));

    if (bind(sockfd,(const struct sockaddr *)&servaddr,sizeof(servaddr)) < 0){
        std::cout << "Bind failed!" << std::endl;
        exit(EXIT_FAILURE);
    }

    while(!stop_listen->load()){
        int n = recvfrom(sockfd,buffer,sizeof(buffer),MSG_WAITALL,(struct sockaddr *)&cliaddr,&len);
        if(n == RETURN_MESSAGE_SIZE){
            auto now = std::chrono::high_resolution_clock::now();
            int* values = reinterpret_cast<int*>(buffer);
            rcv_timestamps[values[0]] = now;
            //std::cout << "[RETURN] finished for id: " << values[0] << " | category: " << values[1] << std::endl;
        }
    }

    close(sockfd);
}

node_id_t setup(ServiceClientAPI& capi,int object_size,int object_rate,int data_part_size,int num_data_parts,char* client_ip,std::chrono::high_resolution_clock::time_point** send_timestamps,std::chrono::high_resolution_clock::time_point** rcv_timestamps){
    // affinity sets mapping function
    capi.set_affinity_set_logic(affinity_logic);

    // cpu affinity
    cpu_affinity(4);
 
    // create necessary object pools
    std::cout << "Creating object pools ... "; fflush(stdout);
    create_pool(capi,OBJ_ENTRY_PATH);
    create_pool(capi,OBJ_CONFIG_PATH);
    for(int i=0;i<NUM_CATEGORIES;i++){ 
        create_pool(capi,OBJ_NEW_CATEGORY_PATH OBJ_PATH_SEP + std::to_string(i));
        create_pool(capi,OBJ_OUTPUT_CATEGORY_PATH OBJ_PATH_SEP + std::to_string(i));
        create_pool(capi,OBJ_DATA_CATEGORY_PATH OBJ_PATH_SEP + std::to_string(i));
    }
    std::cout << "done" << std::endl;

    // getting/putting config to decide what to do
    std::cout << "Getting/putting config ... "; fflush(stdout);
    node_id_t client_id = capi.get_my_id();
    set_client_seed(CLIENT_SEED+client_id);

    int parts_to_put = num_data_parts;
    auto res = capi.list_keys(CURRENT_VERSION,true,OBJ_CONFIG_PATH);
    if(capi.wait_list_keys(res).empty()){
        put_config_object(capi,std::string(OBJ_CONFIG_DATA_PART_SIZE),data_part_size);
        put_config_object(capi,std::string(OBJ_CONFIG_NUM_DATA_PARTS),num_data_parts);
    }
    else {
        if(get_config_int(capi,std::string(OBJ_CONFIG_DATA_PART_SIZE)) == data_part_size){
            int config_num_parts = get_config_int(capi,std::string(OBJ_CONFIG_NUM_DATA_PARTS));
            if(config_num_parts < num_data_parts){
                put_config_object(capi,std::string(OBJ_CONFIG_NUM_DATA_PARTS),num_data_parts);
                parts_to_put -= config_num_parts;
            }
            else parts_to_put = 0;
        }
    }
    put_config_object(capi,std::string(OBJ_CONFIG_CLIENT_DATA) + std::to_string(client_id),client_ip);
    std::cout << "done" << std::endl;

    std::cout << "Putting data objects ... "; fflush(stdout);

    // entry data
    for(int i=num_data_parts-parts_to_put;i<num_data_parts;i++){
        std::string data_key = OBJ_ENTRY_PATH OBJ_PATH_SEP "data_" + std::to_string(i);
        put_random_object(capi,data_key,data_part_size);
    }

    // categories data
    for(int c=0;c<NUM_CATEGORIES;c++){
        for(int i=num_data_parts-parts_to_put;i<num_data_parts;i++){
            std::string data_key = OBJ_DATA_CATEGORY_PATH OBJ_PATH_SEP + std::to_string(c) + OBJ_PATH_SEP + std::to_string(i);
            put_random_object(capi,data_key,data_part_size);
        }
    }
    std::cout << "done" << std::endl;

    // allocate timestamps arrays
    int num_expected_objects = BENCHMARK_TIME * object_rate * 1.1;
    *send_timestamps = new std::chrono::high_resolution_clock::time_point[num_expected_objects];
    *rcv_timestamps = new std::chrono::high_resolution_clock::time_point[num_expected_objects];

    return client_id;
}

int benchmark(ServiceClientAPI& capi,int object_size,int object_rate,int data_part_size,int num_data_parts,node_id_t client_id,std::chrono::high_resolution_clock::time_point* send_timestamps){
    std::cout << "Starting benchmark ..." << std::endl;

    auto period = std::chrono::nanoseconds(1000000000) / object_rate;
    auto stop = std::chrono::seconds(BENCHMARK_TIME);
    auto acc = std::chrono::nanoseconds(0);

    int object_id = 0;
    while(acc < stop){
        auto start = std::chrono::high_resolution_clock::now();

        // put new object
        std::string key = OBJ_ENTRY_PATH OBJ_PATH_SEP + std::to_string(client_id) + "_" + std::to_string(object_id);
        send_timestamps[object_id] = put_random_object(capi,key,object_size);
        
        // sleep
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        if(elapsed < period) std::this_thread::sleep_for(period - elapsed);

        acc += std::chrono::high_resolution_clock::now() - start;
        object_id++;
    }
    
    std::cout << "Benchmark finished!" << std::endl;

    return object_id;
}

void measurements(int num_objects,int object_size,int object_rate,int data_part_size,int num_data_parts,node_id_t client_id,std::chrono::high_resolution_clock::time_point* send_timestamps,std::chrono::high_resolution_clock::time_point* rcv_timestamps){
    std::cerr << client_id << " " << object_size << " " << object_rate << " " << data_part_size << " " << num_data_parts << " " << num_objects << std::endl;
    for(int i=0;i<num_objects;i++){
        auto exp_time = std::chrono::duration_cast<std::chrono::microseconds>(send_timestamps[i] - send_timestamps[0]);
        auto latency = std::chrono::duration_cast<std::chrono::microseconds>(rcv_timestamps[i] - send_timestamps[i]);
        std::cerr << i << " " << latency.count() << " " << exp_time.count() << std::endl;
    }
}

int main(int argc, char** argv) {
    if(!usage(argc,argv)) return 0;

    // benchmark parameters: <object_size> <object_rate> <data_part_size> <num_data_parts> <client_ip_address>   
    int object_size = std::stoi(argv[1]);
    int object_rate = std::stoi(argv[2]);
    int data_part_size = std::stoi(argv[3]);
    int num_data_parts = std::stoi(argv[4]);
    char * client_ip = argv[5];

    // timestamps
    std::chrono::high_resolution_clock::time_point* send_timestamps;
    std::chrono::high_resolution_clock::time_point* rcv_timestamps;

    // connect to service
    std::cout << "Connecting to Cascade ... "; fflush(stdout);
    ServiceClientAPI capi;
    std::cout << "done" << std::endl;

    // setup benchmark: object pools, data, measurement data structures
    node_id_t client_id = setup(capi,object_size,object_rate,data_part_size,num_data_parts,client_ip,&send_timestamps,&rcv_timestamps);

    // listen for UDP notifications
    std::cout << "Starting to listen UDP notifications ... "; fflush(stdout);
    std::atomic<bool> stop_listen(false);
    std::thread listener(listen_notifications,rcv_timestamps,&stop_listen,client_id,client_ip); 
    std::cout << "done" << std::endl;
   
    // run benchmark 
    int num_objects = benchmark(capi,object_size,object_rate,data_part_size,num_data_parts,client_id,send_timestamps);
    std::this_thread::sleep_for(std::chrono::seconds(CLIENT_RETURN_TIMEOUT));
    stop_listen = true;
    listener.join();
    
    // measurements
    measurements(num_objects,object_size,object_rate,data_part_size,num_data_parts,client_id,send_timestamps,rcv_timestamps);

    return 0;
}

