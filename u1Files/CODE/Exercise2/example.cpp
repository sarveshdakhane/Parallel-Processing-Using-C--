#include <iostream>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <Windows.h>
int gettimeofday(struct timeval* tp, struct timezone* tzp)
{
    // Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
    // This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
    // until 00:00:00 January 1, 1970 
    static const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

    SYSTEMTIME  system_time;
    FILETIME    file_time;
    uint64_t    time;

    GetSystemTime(&system_time);
    SystemTimeToFileTime(&system_time, &file_time);
    time = ((uint64_t)file_time.dwLowDateTime);
    time += ((uint64_t)file_time.dwHighDateTime) << 32;

    tp->tv_sec = (long)((time - EPOCH) / 10000000L);
    tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
    return 0;
}
#else 
#include <sys/time.h>
#endif

#define O gettimeofday(&b,NULL)
#define o b.tv_sec+b.tv_usec*0.000001
#define _(t)do{volatile static double g=0;int i;struct timeval b;O;double s=o;double c;do{for(i=0;i<100000;i++)g+=(i*.99)*(i*.99)*(i*.99)*(i*.99)*(i*.99);O;c=o;}while(c-s<t);}while(0);
#define r(x,y)for(int i=0;i<x;i++)fun##ct##y();
#define c(x)func##t##x();
#define v(_)void f##unct##_(){
v(1)_(0.0003)}v(3)_(0.0001)c(1)}v(2)r(42,1)_(0.05)r(100,3)}v(4)c(3)c(2)_(0.03)}v(7)_(0.1)}v(8)r(10,7)_(0.1)c(2)}v(6)_(0.01)c(8)}v(5)_(0.1)c(6)c(8)c(4)}int main(){r(13,5)_(0.1)return 0;}
