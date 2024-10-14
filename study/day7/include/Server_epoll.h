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
            std::function<void()> func = std::bind(&Server_epoll::make_new_connection, this, socket);
            bool is_acceptor = true;
            epoll_.add(socket.get_fd(), func, is_acceptor);
        }
        ~Server_epoll() noexcept(false)
        {
            std::cout << "Server_epoll free" << std::endl;
        }
        void active()
        {
            epoll_.loop();
        }
    private:

        void make_new_connection(Socket& socket)
        {
            std::cout << "new connection" << std::endl;
            int fd = socket.accept();
            std::function<void()> func = std::bind(&Server_epoll::process_recv_buf, this, fd);
            bool is_acceptor = false;
            epoll_.add(fd, func, is_acceptor);
        }
        void process_recv_buf(int socketfd)
        {
            int bufsize = 1024;
            char buf[bufsize];
            std::string big_buffer;
            big_buffer.clear();
            bzero(&buf, sizeof(buf));
            while(true)
            {    
                //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
                int bytes_read = read(socketfd, buf, sizeof(buf));
                // printf("bytes_read: %d\n",bytes_read);
                if(bytes_read > 0)
                {
                    buf[bytes_read] = '\0';
                    big_buffer.append(buf,bytes_read);
                    write(socketfd,buf,bytes_read);
                } 
                else if(bytes_read == 0)
                {
                    //EOF，客户端断开连接
                    printf("message from client fd %d: %s\n", socketfd, big_buffer.c_str());
                    printf("EOF, client fd %d disconnected\n", socketfd);
                    big_buffer.clear();
                    break;
                }
                else if(bytes_read == -1)
                {
                    if(errno == EWOULDBLOCK || errno == EINTR)
                    {
                        continue;
                    }
                    else 
                    {
                        std::cout << "recv process error!" << std::endl;
                        break;
                    }
                } 
                
            }
            // 多线程要及时关闭，以免出现过多close_wait
            // this->epoll_.del(socketfd);
            close(socketfd);   //关闭socket会自动将文件描述符从epoll树上移除
        }
    private:
        Epoll epoll_;
};


