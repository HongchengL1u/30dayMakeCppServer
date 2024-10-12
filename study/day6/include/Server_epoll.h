#pragma once
#include <string.h>
#include <errno.h>
#include <functional>

#include "Socket.h"
#include "Epoll.h"

class Server_epoll
{
    public:
        Server_epoll(Socket& socket):socket_(socket)
        {  
            std::function<void()> func = std::bind(&Server_epoll::make_new_connection, this, &(socket_));
            epoll_.add(socket_.get_fd(), func);
        }
        // ~Server_epoll() noexcept(false)
        // {
        // }
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
                    printf("message from client fd %d: %s\n", socketfd, buf);
                    buf[bytes_read] = '\0';
                    big_buffer += std::string(buf);
                } else if(bytes_read == -1 && errno == EINTR){  //客户端正常中断、继续读取
                    printf("continue reading");
                    continue;
                } else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){//非阻塞IO，这个条件表示数据全部读取完毕
                    printf("finish reading once, errno: %d\n", errno);
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
        Socket& socket_;
        Epoll epoll_;
};


