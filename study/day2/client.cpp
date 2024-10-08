#include "cap_socket.h"

int main()
{
    cap_socket client_1;
    client_1.cap_bind("127.0.0.1",4444);
    client_1.cap_connect("127.0.0.1",5555);
}
