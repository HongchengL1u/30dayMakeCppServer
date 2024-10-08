#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>


#include <netinet/in.h>
#include <arpa/inet.h>


#include <string.h>
// client
// init socket  
// int socket(int domain, int type, int protocol);
// make connections


int main()
{
    // 最后一个参数改变
    // 主要当协议族不明确时，一般为0
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if(socketfd == -1)
    {
        perror("client socket init failed !");
    }
    
    struct sockaddr_in client_addr;
    memset(&client_addr, 0 ,sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(3333); // 16->NBO
    client_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(bind(socketfd, (const struct sockaddr *)(&client_addr),sizeof(struct sockaddr)) == -1)
    {
        perror("client socket bind failed !");
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0 ,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(4444); // 16->NBO
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(connect(socketfd, (sockaddr *)(&server_addr), sizeof(server_addr)) == -1)
    {
        perror("client socket connect failed !");
    }

    #define BUF_SIZE 1000
    char buf[BUF_SIZE];
    char msg[] = "hello world!";
    strcpy(buf, msg);
    buf[strlen(msg)] = '\0';
    // 发送端将字符串的全部内容包括尾部的终止符也发送给客户端

    if(send(socketfd, buf, strlen(buf)+1, 0) == -1) 
    {
        perror("client socket send failed !");
    }
    // int recv_msg_size = recv(socketfd, buf, 1000, 0);
    // if(recv_msg_size == -1)
    // {
    //     perror("server socket recv failed !");
    // }
    // else
    // {
    //     printf("the msg is: %s\nthe size of it is %d\n",buf,recv_msg_size);
    // }
    
    shutdown(socketfd,SHUT_RDWR); 
    // man 2 shutdown
    // 使用close中止一个连接，但它只是减少描述符的参考数，并不直接关闭连接，只有当描述符的参考数为0时才关闭连接。
    // shutdown可直接关闭描述符，不考虑描述符的参考数，可选择中止一个方向的连接。

}
