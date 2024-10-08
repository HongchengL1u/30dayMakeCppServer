#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <string.h>
// client
// init socket  
// int socket(int domain, int type, int protocol);
// make connections

#include <iostream>
#include <vector>

class cap_socket
{
    public:
        cap_socket()
        {
            memset(buf, 0, 1000);
            // 最后一个参数改变
            // 主要当协议族不明确时，一般为0
            socketfd = socket(AF_INET, SOCK_STREAM, 0);
            true_exit(socketfd == -1, "socket init failed !");
            printf("create a socket id == %d\n", socketfd);
        }

        void cap_bind(const char* ip, int port)
        {
            struct sockaddr_in client_addr;
            memset(&client_addr, 0 ,sizeof(client_addr));
            client_addr.sin_family = AF_INET;
            client_addr.sin_port = htons(port); // 16->NBO
            client_addr.sin_addr.s_addr = inet_addr(ip);
            // 复用选项需要在bind之前
            true_exit(setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &client_addr, sizeof(client_addr)) == -1, "server socket bind failed !");
            true_exit(bind(socketfd, (const struct sockaddr *)(&client_addr),sizeof(struct sockaddr)) == -1, "socket bind failed !");
            
        }
        void cap_connect(const char* ip, int port)
        {
            struct sockaddr_in server_addr;
            memset(&server_addr, 0 ,sizeof(server_addr));
            server_addr.sin_family = AF_INET;
            server_addr.sin_port = htons(port); // 16->NBO
            server_addr.sin_addr.s_addr = inet_addr(ip);
            true_exit(connect(socketfd, (sockaddr *)(&server_addr), sizeof(server_addr)) == -1, "socket connect failed !");

            // chatloop();
            // char msg[100];
            // while(scanf("%s",msg))
            // {
            //     printf("%s",msg);
            //     true_exit(write(socketfd, msg, strlen(msg)+1)==-1,"write error!");
            // }
        }
        void cap_send(const char* msg, int other_fd=-1)
        {
            
            if(strlen(msg)+1 > 1000)
            {
                printf("too big msg !\n");
                fflush(stdout);
                return ;
            }
            else
            {
                // 发送端将字符串的全部内容包括尾部的终止符也发送给客户端
                // strcpy 只会将一部分进行赋值, 相比于memcpy复制到'\0'终止
                strcpy(buf, msg);
                buf[strlen(msg)] = '\0';
                if(other_fd!=-1)
                {
                    true_exit(send(other_fd, buf, strlen(buf)+1, 0) == -1, "socket send failed !");
                }
                else true_exit(send(socketfd, buf, strlen(buf)+1, 0) == -1, "socket send failed !");
            }
        }
        void cap_listen(int num)
        {
            true_exit(listen(socketfd,num) == -1, "server socket listen failed !");
        }
        void cap_accept()
        {
            struct sockaddr_in client_addr;
            socklen_t len;
            bzero(&client_addr, sizeof(client_addr));
            // 每次都要zero一下，不然会出现问题
            int client_socket_fd = accept(socketfd, (sockaddr *)&client_addr, &len);
            true_exit(client_socket_fd == -1, "server socket accept failed !");
            printf("build a connection from %s:%d\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
            // cap_recv(client_socket_fd);
            // cap_send("hello, im server_1!",client_socket_fd);
            while(1)
            {
                int read_bytes_num = read(client_socket_fd, buf, 1000);
                true_exit(read_bytes_num == -1, "read error!");
                if(read_bytes_num == 0) break;
                printf("the msg is: %s\nthe size of it is %d\n",buf,read_bytes_num);
            }
            
            // struct data_process_struct temp = {&client_socket_fd};
            // data_process_feedback(&temp);
            // shutdown(client_socket_fd,SHUT_RDWR);
        }

        // struct data_process_struct
        // {
        //     const int* socketfd_ptr;
        // };
        // void* data_process_feedback(void* arg)
        // {
        //     const int socketfd = *(((struct data_process_struct*)(arg))->socketfd_ptr);
        //     int recv_msg_size = recv(socketfd, buf, 1000, 0); // 这里使用连接fd而非监听fd
        //     true_exit(recv_msg_size == -1, "server socket recv failed !");
        //     printf("the msg is: %s\nthe size of it is %d\n",buf,recv_msg_size);
        // }

        void cap_recv(int other_fd = -1)
        {
            int recv_msg_size = -1;
            if(other_fd!=-1)
            {
                recv_msg_size = recv(other_fd, buf, 1000, 0);
            }
            else recv_msg_size = recv(socketfd, buf, 1000, 0);
            true_exit(recv_msg_size == -1, "socket recv failed !");
            printf("the msg is: %s\nthe size of it is %d\n",buf,recv_msg_size);
        }
        ~cap_socket () noexcept(false) //表示异常可以被外部捕获
        {
            // 析构函数会在栈展开时运行，此时出现异常，不会被捕获
            // 但是当存在异常时，析构函数也出现异常，此时会直接非正常退出
            // 析构函数应该设计为不抛出异常
            shutdown(socketfd,SHUT_RDWR);
            // man 2 shutdown
            // 使用close中止一个连接，但它只是减少描述符的参考数，并不直接关闭连接，只有当描述符的参考数为0时才关闭连接。
            // shutdown可直接关闭描述符，不考虑描述符的参考数，可选择中止一个方向的连接。
        }
        void true_exit(bool cond, const char* show_msg)
        {
            if(cond)
            {
                perror(show_msg);
                // 析构函数
                // 只有这样才会正确调用析构函数
                this->~cap_socket();
                exit(0); // return是函数的返回，exit是进程的退出
                // exit会调用全局变量的析构函数，但是局部变量的不会调用
            }   
        }
    private:
        char buf[1000];
        int socketfd = -1;
        // std::vector<int> accept_list; // 怎么处理？多accept
};