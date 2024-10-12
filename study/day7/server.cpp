#include "include/Server_epoll.h"
#include "include/Socket.h"
int main()
{
    Socket socket;
    socket.bind("127.0.0.1",5555);
    if(socket.get_fd() == -1) return 0;
    socket.listen();
    Server_epoll server(socket);
    server.active();
    return 0;
}   