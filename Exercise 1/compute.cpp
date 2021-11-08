#include <iostream>
#include <iomanip>
#include <math.h>
#include <chrono>
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>

using namespace std;

typedef unsigned long long ull;
std::mutex m;
std::mutex m1;
std::mutex m2;
std::condition_variable cv;
std::condition_variable cv1;
std::condition_variable cv2;
bool ready = false;

extern ull f1();
extern ull f2(ull a);
extern ull f3(ull a);
extern ull f4(ull a);
extern ull f5(ull b);
extern ull f6(ull b, ull c);
extern ull f7(ull a);

ull f1()
{
	cout<<"Called F1 "<<endl;
	return 1900000000;
}
ull f2(ull a)
{
	std::unique_lock<std::mutex> lck(m);
	cout<<"Called F2 "<<endl;
	double Oddsum=0;
	for (ull i=0; i<=1900000000; ++i)
	{
		if((i&1)==0)
		{
			Oddsum +=i;

		}
	}
	return a * Oddsum;
	cv.notify_all();
}
ull f3(ull a)
{
	std::unique_lock<std::mutex> lck(m2);
	cout<<"Called F3 "<<endl;
	ull Evensum=0;
	for (ull i=0; i<=1900000000; ++i)
	{
		if((i&1)==0)
		{
			Evensum +=i;
     	}
	}
	
	return a * Evensum;
	cv2.notify_all();

}
ull f4(ull a)
{
	std::unique_lock<std::mutex> lck(m1);
	cout<<"Called F4"<<endl ;
	return a+9009925000;
	cv1.notify_all();
}
ull f5(ull b)
{
    std::unique_lock<mutex> lck (m);
    cv.wait(lck , [&]{ if (b > 0 ){ return true ;} else { return false ;}});
	cout<<"Called F5"<<endl ;
	return b*19222540000;
}
ull f6(ull b,ull c)
{
	std::unique_lock<mutex> lck1 (m);
	std::unique_lock<mutex> lck1 (m2);
	cv.wait(lck1, [&]{ if (b > 0){ return true ;} else { return false ;}});
    cv2.wait(lck1, [&]{ if (c> 0){ return true ;} else { return false ;}});
	cout<<"Called F6"<<endl ;
	return b*c;
}
ull f7(ull a)
{
	std::unique_lock<mutex> lck2 (m1);
    cv1.wait(lck2 , [&]{ if (a > 0 ){ return true ;} else { return false ;}});
	cout<<"Called F7"<<endl;
	return a +19007193425;
}


int main(int argc, char **argv)
{
	cout<<"Hello Ji..."<<endl;
	
	ull a, a1,b, c, res;
	auto start = std::chrono::high_resolution_clock::now();
	
	/*
    a = f1();
	auto Future_f2=std::async(f2,a);
	auto Future_f3=std::async(f3,a);
	auto Future_f4=std::async(f4,a);
	b=Future_f2.get(); 
	c=Future_f3.get();	
    a1 = Future_f4.get(); 
    auto Future_f5 = std::async(f5,b); 
	auto Future_f7 = std::async(f7,a1); 
	auto Future_f6 = std::async(f6,b,c);
    res = Future_f5.get() + Future_f7.get() + Future_f6.get();
	*/

	
	a = f1();
    b = f2(a);	
	c = f3(a);
	a = f4(a);
	res = f5(b) + f7(a) + f6(b,c);
	
    
	auto end = std::chrono::high_resolution_clock::now();
	
	cout << "Result = " << std::setprecision(18) << res << endl;
	cout << "Time: " << (end - start)/std::chrono::milliseconds(1) << "ms" << endl;
	return 0;
}
