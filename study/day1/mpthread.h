#pragma once
#include <pthread.h>

class mp
{
    public:
        mp(void *(*func) (void *),void *arg)
        {
            pthread_create(&p,nullptr,func,arg);
        }
        ~mp()
        {
            pthread_detach(p);
        }
    private:
        pthread_t p;
};  
