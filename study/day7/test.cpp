#include <iostream>
#include <unistd.h>
#include <string.h>
#include <functional>


#include "include/Socket.h"
#include "include/Threadpool.h"

using namespace std;

void oneClient(int msgs, int wait){
    Socket client;
    // client.bind("127.0.0.1",port);
    client.connect("127.0.0.1",5555);

    // Socket *sock = new Socket();
    // InetAddress *addr = new InetAddress("127.0.0.1", 1234);
    // sock->connect(addr);

    int sockfd = client.get_fd();
    std::string readbuffer;
    std::string sendbuffer;
    sleep(wait);
    int count = 0;
    while(count < msgs){
        sendbuffer.append("I'm client!");
        ssize_t write_bytes = write(sockfd, sendbuffer.c_str(), sendbuffer.size());
        if(write_bytes == -1){
            printf("server socket already disconnected, can't write any more!\n");
            break;
        }
        int already_read = 0;
        char buf[1024];    //这个buf大小无所谓
        while(true){
            bzero(&buf, sizeof(buf));
            ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
            if(read_bytes > 0){
                readbuffer.append(buf, read_bytes);
                already_read += read_bytes;
            } else if(read_bytes == 0){         //EOF
                printf("server disconnected!\n");
                exit(EXIT_SUCCESS);
            }
            if(already_read >= sendbuffer.size()){
                printf("count: %d, message from server: %s\n", count++, readbuffer.c_str());
                break;
            } 
        }
        readbuffer.clear();
    }
}

int main(int argc, char *argv[]) {
    int threads = 100;
    int msgs = 100;
    int wait = 0;
    int o;
    const char *optstring = "t:m:w:";
    while ((o = getopt(argc, argv, optstring)) != -1) {
        switch (o) {
            case 't':
                threads = stoi(optarg);
                break;
            case 'm':
                msgs = stoi(optarg);
                break;
            case 'w':
                wait = stoi(optarg);
                break;
            case '?':
                printf("error optopt: %c\n", optopt);
                printf("error opterr: %d\n", opterr);
                break;
        }
    }

    Threadpool *pool = new Threadpool(threads);
    std::function<void()> func = std::bind(oneClient, msgs, wait);
    for(int i = 0; i < threads; ++i){
        pool->add(func);
    }
    delete pool;
    return 0;
}
