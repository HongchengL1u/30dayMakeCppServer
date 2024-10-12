#pragma once
#include <functional>
#include <sys/epoll.h>
#include <string.h>
#include <fcntl.h>
class Channel
{
    public:
        Channel(int fd, std::function<void()>& func):func_(std::move(func))
        {
            bzero(&ev_, sizeof(ev_));
            ev_.events = EPOLLIN | EPOLLET;
            ev_.data.ptr = this;
            fd_ = fd;
            setnonblocking(fd_); // 需要设置为非阻塞socket通信
        }
        // ~Channel();

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
    private:
        struct epoll_event ev_;
        int fd_;
        std::function<void()> func_;
        
};