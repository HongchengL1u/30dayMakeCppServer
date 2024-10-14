#pragma once
#include <functional>
#include <sys/epoll.h>
#include <string.h>
#include <fcntl.h>
class Channel
{
    public:
        Channel(int fd, std::function<void()>& func, bool is_acceptor):func_(std::move(func)),is_acceptor_(is_acceptor)
        {
            bzero(&ev_, sizeof(ev_));
            ev_.events = EPOLLIN;

            ev_.data.ptr = this;
            fd_ = fd;
            if(!is_acceptor) 
            {
                // 对于客户端连接的处理需要设置为非阻塞socket通信，同时非阻塞就需要打开这个EPOLET，
                // ET模式时，事件就绪时，假设对事件没做处理，内核不会反复通知事件就绪
                set_ET();
                setnonblocking(fd_);
            } 
        }
        // ~Channel();
        void set_ET()
        {
            ev_.events |= EPOLLET;
        }
        struct epoll_event& get_ev()
        {
            return ev_;
        }
        std::function<void()>& get_func()
        {
            return func_;
        }
        int get_fd()
        {
            return fd_;
        }
        void setnonblocking(int fd)
        {
            fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
        }
        bool is_acceptor()
        {
            return is_acceptor_;
        }
    private:
        struct epoll_event ev_;
        int fd_;
        std::function<void()> func_;
        bool is_acceptor_ = false;
        
};