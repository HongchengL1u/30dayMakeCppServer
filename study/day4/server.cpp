#include "Server_epoll.h"

int main()
{
    Server_epoll server;
    server.server_bind("127.0.0.1",5555);
    server.server_epoll_init();
    server.server_epoll_loop();
    return 0;
}   