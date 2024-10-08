#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <netinet/in.h>
#include <arpa/inet.h>


#include <string.h>


#include "mpthread.h"

struct data_process_struct
{
    const int* socketfd_ptr;
};

char buf[1000];

void* data_process_feedback(void* arg)
{
    const int socketfd = *(((struct data_process_struct*)(arg))->socketfd_ptr);
    
    int recv_msg_size = recv(socketfd, buf, 1000, 0); // 这里使用连接fd而非监听fd
    if(recv_msg_size == -1)
    {
        perror("server socket recv failed !");
    }
    else
    {
        printf("the msg is: %s\nthe size of it is %d\n",buf,recv_msg_size);
    }
}

class server_socket
{
    public:
        server_socket()
        {
            // 最后一个参数改变
            // 主要当协议族不明确时，一般为0
            socketfd = socket(AF_INET, SOCK_STREAM, 0);
            true_exit(socketfd==-1, "server socket init failed !");
        }
        ~server_socket()
        {
            shutdown(socketfd,SHUT_RDWR); 
        }
        void bind()
        {
            
        }
        void true_exit(bool cond, const char* show_msg)
        {
            if(cond)
            {
                perror(show_msg);
                exit(-1); // return是函数的返回，exit是进程的退出
            }   
        }
    private:
        int socketfd = -1;
};


int main()
{
    
    struct sockaddr_in server_addr;
    memset(&server_addr, 0 ,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(4444); // 16->NBO
    server_addr.sin_addr.s_addr = inet_addr("172.29.59.167");
    if(setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &server_addr, sizeof(server_addr) ))
    {
        perror("server socket bind failed !");
        return -1;
    }
    if(bind(socketfd, (sockaddr *)(&server_addr),sizeof(server_addr)) == -1)
    {
        perror("server socket bind failed !");
        return -1;
    }

    if(listen(socketfd,5) == -1) // 对UDP无用，面对连接的
    {
        perror("server socket listen failed !");
        return -1;
    }
    // while(1)
    // {
    //     struct sockaddr client_addr;
    //     socklen_t len;
    //     bzero(&len, sizeof(len));
    //     int client_socket_fd = accept(socketfd, &client_addr, &len);
    //     if(client_socket_fd == -1)
    //     {
    //         perror("server socket accept failed !");
    //     }
    //     else
    //     {
    //         struct sockaddr_in* client_addr_2 = (struct sockaddr_in*)(&client_addr);
    //         printf("build a connection from %s:%d\n",inet_ntoa(client_addr_2->sin_addr),ntohs(client_addr_2->sin_port));
    //     }
    //     struct data_process_struct value;
    //     value.socketfd_ptr = &socketfd;
    //     mp p(data_process_feedback,&value);
    // }   
    struct sockaddr_in client_addr;
    socklen_t len;
    bzero(&client_addr, sizeof(client_addr));
    // 每次都要zero一下，不然会出现问题
    int client_socket_fd = accept(socketfd, (sockaddr *)&client_addr, &len);
    if(client_socket_fd == -1)
    {
        perror("server socket accept failed !");
        return -1;
    }
    else
    {
        printf("build a connection from %s:%d\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
    }
    struct data_process_struct value;
    value.socketfd_ptr = &client_socket_fd;
    if(data_process_feedback(&value))
    {
        return -1;
    }
    
    // man 2 shutdown
    // 使用close中止一个连接，但它只是减少描述符的参考数，并不直接关闭连接，只有当描述符的参考数为0时才关闭连接。
    // shutdown可直接关闭描述符，不考虑描述符的参考数，可选择中止一个方向的连接。

}
