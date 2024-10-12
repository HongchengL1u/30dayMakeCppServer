#include <iostream>
#include <unistd.h>
#include <string.h>
#include <functional>


#include "include/Socket.h"
#include "include/Threadpool.h"

using namespace std;

void oneClient(int msgs, int wait)
{
    Socket client;
    // client.bind("127.0.0.1",port);
    client.connect("127.0.0.1",5555);

    int sockfd = client.get_fd();
    if(sockfd == -1) return;
    std::string readbuffer;
    std::string sendbuffer;
    // std::cout << "wait:" << wait << std::endl;
    // sleep(wait);
    int count = 0;
    while(count < msgs)
    {
        sendbuffer.append("I'm client! "+std::to_string(sockfd));
        ssize_t write_bytes = write(sockfd, sendbuffer.c_str(), sendbuffer.size());
        if(write_bytes == -1){
            printf("server socket already disconnected, can't write any more!\n");
            break;
        }
        int have_read = 0;
        char buf[1024];    //这个buf大小无所谓
        while(true)
        {
            bzero(&buf, sizeof(buf));
            ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
            if(read_bytes > 0)
            {
                readbuffer.append(buf,read_bytes);
                have_read+=read_bytes;
            } 
            else
            {
                std::cout << "read over!" << std::endl;
            }
            if(have_read>=sendbuffer.size())
            {
                std::cout << "recv from server " << readbuffer << std::endl;
                break;
            }
        }
        count++;
        // while(true)
        // {
        //     bzero(&buf, sizeof(buf));
        //     ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
        //     if(read_bytes > 0)
        //     {
        //         buf[read_bytes] = '\0';
        //         readbuffer.append(buf);
        //     } 
        //     else if(read_bytes == 0)
        //     {
        //         //EOF         
        //         std::cout << "read over!" << std::endl;
        //         break;
        //     }
        //     else
        //     {
        //         std::cout << "read error!" << std::endl;
        //         break;
        //     }
        // } 
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
    printf("%d %d %d\n",threads,msgs,wait);
    Threadpool *pool = new Threadpool(threads);
    std::function<void()> func = std::bind(oneClient, msgs, wait);
    for(int i = 0; i < threads; ++i){
        pool->add(func);
    }
    delete pool;
    return 0;
}
