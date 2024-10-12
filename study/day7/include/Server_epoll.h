#pragma once
#include <string.h>
#include <errno.h>

#include <functional>
#include <iostream>
#include <csignal> 

#include "Socket.h"
#include "Epoll.h"

void handleSignal(int signal) 
{
    std::cout << "捕捉到信号: " << signal << ", 准备退出..." << std::endl;
    // 在这里添加清理资源的代码
}

class Server_epoll
{
    public:
        Server_epoll(Socket& socket)
        {  
            signal(SIGINT, handleSignal);
            std::function<void()> func = std::bind(&Server_epoll::make_new_connection, this, &(socket));
            epoll_.add(socket.get_fd(), func);
        }
        ~Server_epoll() noexcept(false)
        {
        }
        void active()
        {
            epoll_.loop();
        }
    private:

        void make_new_connection(Socket* s_ptr)
        {
            std::cout << "new connection" << std::endl;
            int fd = s_ptr->accept();
            std::function<void()> func = std::bind(&Server_epoll::process_recv_buf, this, fd);
            epoll_.add(fd, func);
        }
        void process_recv_buf(int socketfd)
        {
            int bufsize = 1024;
            char buf[bufsize];
            std::string big_buffer;
            big_buffer.clear();
            while(true)
            {    
                //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
                bzero(&buf, sizeof(buf));
                int bytes_read = read(socketfd, buf, sizeof(buf));
                if(bytes_read > 0){
<<<<<<< HEAD
                    printf("message from client fd %d\n", socketfd);
                    buf[bytes_read] = '\0';
                    big_buffer.append(buf);
=======
                    printf("message from client fd %d: %s\n", socketfd, buf);
                    buf[bytes_read] = '\0';
                    big_buffer += std::string(buf);
>>>>>>> b407d05027e98a03897e5bad6e1f0c41237a9cf8
                } else if(bytes_read == -1 && errno == EINTR){  //客户端正常中断、继续读取
                    printf("continue reading");
                    continue;
                } else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){//非阻塞IO，这个条件表示数据全部读取完毕
<<<<<<< HEAD
                    printf("finish reading once, msg: %s\n", big_buffer.c_str());
=======
                    printf("finish reading once, errno: %d\n", errno);
>>>>>>> b407d05027e98a03897e5bad6e1f0c41237a9cf8
                    write(socketfd, big_buffer.c_str(), big_buffer.size());
                    big_buffer.clear();
                    break;
                } else if(bytes_read == 0){  
                    //EOF，客户端断开连接
                    printf("EOF, client fd %d disconnected\n", socketfd);
                    close(socketfd);   //关闭socket会自动将文件描述符从epoll树上移除
                    break;
                }
            }
        }
    private:
        Epoll epoll_;
};


