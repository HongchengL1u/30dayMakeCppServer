#include "include/Socket.h"
#include <cassert>

int main(int argc, char** argv)
{
    // assert(argc == 2);
    // int port = atoi(argv[1]);
    Socket client;
    // client.bind("127.0.0.1",port);
    client.connect("127.0.0.1",5555);
}