#include <string.h>
#include <iostream>


void p(const char buf[100])
{
    for(int i=0;i<10;i++)
    {
        for(int j=0;j<10;j++)
        {
            std::cout << buf[i*10+j] << " ";
        }
        std::cout << std::endl;
    }
}

int main()
{
    char buf[100];
    bzero(buf,100);
    p(buf);
    char* msg = "hello world!";
    strcpy(buf,msg);
    p(buf);
    msg = "over!";
    strcpy(buf,msg);
    p(buf);

    
}