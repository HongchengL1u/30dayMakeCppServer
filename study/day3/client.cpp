#include "client_socket.h"
#include <cassert>

int main(int argc, char** argv)
{
    assert(argc == 2);
    int port = atoi(argv[1]);
    client_socket client;
    client.cap_bind("127.0.0.1",port);
    client.cap_connect("127.0.0.1",5555);
    
}