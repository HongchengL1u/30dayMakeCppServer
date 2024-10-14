#pragma once
#include <string.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <unistd.h>

#include <vector>
#include <functional>
#include "Channel.h"
#include "Threadpool.h"


class Epoll
{
    public:
        Epoll()
        {
            thread_pool_ptr = new Threadpool(5);
            events = new epoll_event[max_events_num];
            memset(events, 0, max_events_num*sizeof(epoll_event));
            epfd = epoll_create1(0);
            true_exit(epfd == -1, "epoll create error!");
        }
        ~Epoll()
        {
            std::cout << "epoll free" <<std::endl;
            delete thread_pool_ptr;
            delete[]events;
            for(auto& el:Channel_des)
            {
                delete(el);
            }
            close(epfd);
        }
        void add(int fd, std::function<void()>& func, bool is_acceptor)
        {
            // ev如果被挂载到epfd上，则其上的内容是会被保存的
            // 但是虽然event内容能够指向channel，但是channel是一个局部变量，所以需要一个东西进行存储，并且程序结束时进行消除
            Channel* c_ptr = new Channel(fd,func,is_acceptor);
            Channel_des.emplace_back(c_ptr);
            // 初始化，加入初始事件
            true_exit(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &c_ptr->get_ev())==-1, "epoll add failed!"); // 在epfd上挂载该任务
        }
        void del(int socketfd)
        {
            true_exit(epoll_ctl(epfd, EPOLL_CTL_DEL, socketfd, nullptr)==-1, "epoll delete failed!"); // 在epfd上删除该任务
        }
        void loop()
        {
            while(true)
            {
                int event_available_nums = epoll_wait(epfd, events, max_events_num, -1);
                true_exit(event_available_nums==-1, "epoll_wait error!");
                for(int i=0;i<event_available_nums;i++)
                {
                    Channel* ch = (Channel*)events[i].data.ptr;
                    if(ch->is_acceptor()) (ch->get_func())();
                    else thread_pool_ptr->add(ch->get_func());
                }
            }
        }
    private:
        void true_exit(bool cond, const char* show_msg)
        {
            if(cond)
            {
                perror(show_msg);
                // 析构函数
                // 只有这样才会正确调用析构函数
                this->~Epoll();
                exit(0); // return是函数的返回，exit是进程的退出
                // exit会调用全局变量的析构函数，但是局部变量的不会调用
            }   
        }
       
        int max_events_num = 2048;
        struct epoll_event* events = nullptr;
        int epfd = -1;
        std::vector<Channel*> Channel_des;
        Threadpool* thread_pool_ptr;
};
