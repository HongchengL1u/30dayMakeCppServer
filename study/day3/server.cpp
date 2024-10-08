#include <sys/epoll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <string.h>


#include <unistd.h>
#include <fcntl.h>

#include <errno.h>
#define MAX_EVENTS 1024
#define READ_BUFFER 1024
void true_exit(bool cond, const char* show_msg)
{
    if(cond)
    {
        perror(show_msg);
        // 析构函数
        // 只有这样才会正确调用析构函数
        exit(0); // return是函数的返回，exit是进程的退出
        // exit会调用全局变量的析构函数，但是局部变量的不会调用
    }   
}

void setnonblocking(int fd){
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    true_exit(sockfd == -1, "socket create error!");
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(5555);
    true_exit(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &serv_addr, sizeof(serv_addr)) == -1, "server socket bind failed !");
    true_exit(bind(sockfd,(const sockaddr*)&serv_addr, sizeof(serv_addr)), "socket bind error!");
    true_exit(listen(sockfd, SOMAXCONN) == -1, "socket listen error");
    

    int epfd = epoll_create1(0);
    true_exit(epfd == -1, "epoll create error!");


    struct epoll_event events[MAX_EVENTS], ev;
    bzero(&events, sizeof(events));

    // 初始化，加入初始事件
    bzero(&ev, sizeof(ev));
    ev.data.fd = sockfd;
    ev.events = EPOLLIN | EPOLLET; 
    setnonblocking(sockfd); // 需要设置为非阻塞socket通信
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev); // 在epfd上挂载该任务

    while(true)
    {
        int event_available_nums = epoll_wait(epfd, events, MAX_EVENTS, -1);
        true_exit(event_available_nums==-1, "epoll_wait");
        for(int i=0;i<event_available_nums;i++)
        {
            if(events[i].data.fd == sockfd) // 表明处理新连接
            {
                // 处理连接请求
                struct sockaddr_in client_addr;
                socklen_t len = sizeof(client_addr);
                bzero(&client_addr, sizeof(client_addr));
                // 每次都要zero一下，不然会出现问题
                int client_socket_fd = accept(sockfd, (sockaddr *)&client_addr, &len);
                true_exit(client_socket_fd == -1, "server socket accept failed !");
                printf("build a connection from %s:%d\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));

                // 添加事件，与初始化一样
                bzero(&ev, sizeof(ev));
                ev.data.fd = client_socket_fd;
                ev.events = EPOLLIN | EPOLLET;
                setnonblocking(client_socket_fd);
                epoll_ctl(epfd, EPOLL_CTL_ADD, client_socket_fd, &ev);
            }
            else if(events[i].events & EPOLLIN)
            {
                char buf[READ_BUFFER];
                while(true)
                {    
                    //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
                    bzero(&buf, sizeof(buf));
                    int bytes_read = read(events[i].data.fd, buf, sizeof(buf));
                    if(bytes_read > 0){
                        printf("message from client fd %d: %s\n", events[i].data.fd, buf);
                        write(events[i].data.fd, buf, sizeof(buf));
                    } else if(bytes_read == -1 && errno == EINTR){  //客户端正常中断、继续读取
                        printf("continue reading");
                        continue;
                    } else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){//非阻塞IO，这个条件表示数据全部读取完毕
                        printf("finish reading once, errno: %d\n", errno);
                        break;
                    } else if(bytes_read == 0){  
                        //EOF，客户端断开连接
                        printf("EOF, client fd %d disconnected\n", events[i].data.fd);
                        close(events[i].data.fd);   //关闭socket会自动将文件描述符从epoll树上移除
                        break;
                    }
                }
            }
            else
            {
                printf("something else");
            }
        }
        
    }
    close(sockfd);
    return 0;
}   