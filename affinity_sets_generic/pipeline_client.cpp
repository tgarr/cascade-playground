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
    if(argc >= 4) return true;
    std::cout << argv[0] << " <object_size> <object_rate> <data_size>" << std::endl;
    return false;
}

inline void cpu_affinity(int core_id){
    pthread_t thread = pthread_self();
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id,&cpuset);
    pthread_setaffinity_np(thread,sizeof(cpu_set_t),&cpuset);
}

void listen_notifications(std::chrono::high_resolution_clock::time_point* rcv_timestamps,std::atomic<bool>* stop_listen){
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
    servaddr.sin_port = htons(CLIENT_RETURN_UDP_PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

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
            std::cout << "[RETURN] finished for id: " << values[0] << " | category: " << values[1] << std::endl;
        }
    }

    close(sockfd);
}

void setup(ServiceClientAPI& capi,int object_size,int object_rate,int data_size,std::chrono::high_resolution_clock::time_point** send_timestamps,std::chrono::high_resolution_clock::time_point** rcv_timestamps){
    // affinity sets mapping function
    capi.set_affinity_set_logic(affinity_logic);

    // cpu affinity
    cpu_affinity(4);

    // create necessary object pools
    std::cout << "Creating object pools ... "; fflush(stdout);
    create_pool(capi,OBJ_ENTRY_PATH);
    create_pool(capi,OBJ_DATA_CATEGORY_PATH);
    for(int i=0;i<NUM_CATEGORIES;i++){ 
        create_pool(capi,OBJ_NEW_CATEGORY_PATH OBJ_PATH_SEP + std::to_string(i));
        create_pool(capi,OBJ_OUTPUT_CATEGORY_PATH OBJ_PATH_SEP + std::to_string(i));
    }
    std::cout << "done" << std::endl;

    std::cout << "Putting data objects ... "; fflush(stdout);

    // entry data
    put_random_object(capi,OBJ_ENTRY_PATH OBJ_PATH_SEP + std::string("data"),data_size);

    // categories data
    for(int i=0;i<NUM_CATEGORIES;i++){
        put_random_object(capi,OBJ_DATA_CATEGORY_PATH OBJ_PATH_SEP + std::to_string(i),data_size);
    }
    std::cout << "done" << std::endl;

    // allocate timestamps arrays
    int num_expected_objects = BENCHMARK_TIME * object_rate * 1.1;
    *send_timestamps = new std::chrono::high_resolution_clock::time_point[num_expected_objects];
    *rcv_timestamps = new std::chrono::high_resolution_clock::time_point[num_expected_objects];
}

int benchmark(ServiceClientAPI& capi,int object_size,int object_rate,int data_size,std::chrono::high_resolution_clock::time_point* send_timestamps){
    std::cout << "Starting benchmark ..." << std::endl;

    auto period = std::chrono::nanoseconds(1000000000) / object_rate;
    auto stop = std::chrono::seconds(BENCHMARK_TIME);
    auto acc = std::chrono::nanoseconds(0);

    int object_id = 0;
    while(acc < stop){
        auto start = std::chrono::high_resolution_clock::now();

        // put new object
        std::string key = OBJ_ENTRY_PATH OBJ_PATH_SEP + std::to_string(object_id);
        put_random_object(capi,key,object_size);
        
        // save timestamp
        auto now = std::chrono::high_resolution_clock::now();
        send_timestamps[object_id] = now;

        // sleep
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        if(elapsed < period) std::this_thread::sleep_for(period - elapsed);

        acc += std::chrono::high_resolution_clock::now() - start;
        object_id++;
    }
    
    std::cout << "Benchmark finished!" << std::endl;

    return object_id;
}

void measurements(std::chrono::high_resolution_clock::time_point* send_timestamps,std::chrono::high_resolution_clock::time_point* rcv_timestamps,int num_objects,int object_size,int object_rate,int data_size){
    std::cerr << object_size << " " << object_rate << " " << data_size << " " << num_objects << std::endl;
    for(int i=0;i<num_objects;i++){
        auto latency = std::chrono::duration_cast<std::chrono::microseconds>(rcv_timestamps[i] - send_timestamps[i]);
        std::cerr << i << " " << latency.count() << std::endl;
    }
}

int main(int argc, char** argv) {
    if(!usage(argc,argv)) return 0;

    // benchmark parameters   
    int object_size = std::stoi(argv[1]); 
    int object_rate = std::stoi(argv[2]); 
    int data_size = std::stoi(argv[3]); 

    // timestamps
    std::chrono::high_resolution_clock::time_point* send_timestamps;
    std::chrono::high_resolution_clock::time_point* rcv_timestamps;

    // connect to service
    std::cout << "Connecting to Cascade ... "; fflush(stdout);
    ServiceClientAPI capi;
    std::cout << "done" << std::endl;

    // setup benchmark: object pools, data, measurement data structures
    setup(capi,object_size,object_rate,data_size,&send_timestamps,&rcv_timestamps);

    // listen for UDP notifications
    std::cout << "Starting to listen UDP notifications ... "; fflush(stdout);
    std::atomic<bool> stop_listen(false);
    std::thread listener(listen_notifications,rcv_timestamps,&stop_listen); 
    std::cout << "done" << std::endl;
   
    // run benchmark 
    int num_objects = benchmark(capi,object_size,object_rate,data_size,send_timestamps);
    stop_listen = true;
    listener.join();
    
    // measurements
    measurements(send_timestamps,rcv_timestamps,num_objects,object_size,object_rate,data_size);

    return 0;
}

