#include "cap_socket.h"

int main()
{
    
    cap_socket server_1;
    server_1.cap_bind("127.0.0.1",5555);
    server_1.cap_listen(5);
    server_1.cap_accept();
}
