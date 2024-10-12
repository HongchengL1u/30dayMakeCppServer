#pragma once

#include <thread>
#include <iostream>
#include <mutex>
#include <vector>
#include <queue>
#include <condition_variable>
#include <functional>
class Threadpool {
public:
    Threadpool(int num):stop(false)
    {
        for(int i=0;i<num;i++)
        {
            threads.emplace_back(std::thread([this]()
            {
                while (true)
                {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(mutex);
                        cv.wait(lock,[this](){return stop || !tasks.empty();});
<<<<<<<<< Temporary merge branch 1
                        if(stop&&tasks.empty()) return;
=========
                        if(stop && tasks.empty()) return;
>>>>>>>>> Temporary merge branch 2
                        task = tasks.front();
                        tasks.pop();
                    }
                    task();
                }
            }));
        }
    }
    ~Threadpool()
    {
        std::cout << "threadpool free!" << std::endl;
        {
            std::unique_lock<std::mutex> lock(mutex);
            stop = true;
        }
        cv.notify_all();
        // 唤醒所有线程，并重新进行一次判断
        for(auto& t:threads)
        {
            if(t.joinable()) t.join();
        }
    }
    void add(std::function<void()> func)
    {
        {
            std::unique_lock<std::mutex> lock(mutex);
            if(stop)
            {
                std::cout<<"ThreadPool already stop, can't add task any more"<<std::endl;
                return;
            }
            tasks.push(func);
        }
        cv.notify_one(); // 生产者通知消费者，生产者属于主线程
    }
private:
    bool stop;
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    std::mutex mutex;
    std::condition_variable cv;
};