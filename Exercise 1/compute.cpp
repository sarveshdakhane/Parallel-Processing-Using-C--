#include <iostream>
#include <iomanip>
#include <math.h>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "functions.h"

using namespace std;

std::mutex m;
std::mutex m1;
std::mutex m2;
std::condition_variable cv;
std::condition_variable cv1;
std::condition_variable cv2;

int main(int argc, char **argv)
{
	cout << "Runnig..." << endl;
	double a, a1, b, c, res;
	auto start = std::chrono::high_resolution_clock::now();

    ///* 
    a = f1();

    std::thread F2{[&](){ 
		std::lock_guard<std::mutex> lck(m);
		b=f2(a);
        cv.notify_all();
   	}};
	
	std::thread F3{[&](){ 
		std::lock_guard<std::mutex> lck2(m2);
		c=f3(a);
        cv2.notify_all();
   	}};
    
	std::thread F4{[&](){ 
		std::lock_guard<std::mutex> lck1(m1);
		a1=f4(a);
        cv1.notify_all();
   	}};
    
	std::thread F5{[&](){ 
		std::unique_lock<mutex> lck(m);
		cv.wait(lck, [&]()
			{
				if (b != 0)
				{
					return true;
				}
				else
				{
					return false;
				}
			});
		res+=f5(b);
   	}};
   
	std::thread F7{[&](){ 
	    std::unique_lock<mutex> lck1(m1);
		cv1.wait(lck1, [&]()
			{
				if (a1 != 0)
				{
					return true;
				}
				else
				{
				    return false;
				}
			});
		res+=f7(a1);
   	}};
  
	std::thread F6{[&](){ 
			std::unique_lock<mutex> lck(m);
	        std::unique_lock<mutex> lck2(m2);
			cv.wait(lck, [&]()
			{
				if (b != 0)
				{
					return true;
				}
				else
				{
					return false;
				}
			});
			cv2.wait(lck2, [&]()
			 {
				 if (c != 0)
				 {
					 return true;
				 }
				 else
				 {
					 return false;
				 }
			 });
			res+=f6(b,c);
		}};
 
     F2.join();
	 F3.join();
	 F4.join();
     F5.join();
	 F7.join();
	 F6.join();

	//*/
	/*
	a = f1();
	b = f2(a);
	c = f3(a);
	a = f4(a);
	res = f5(b) + f7(a) + f6(b, c);
	*/

	auto end = std::chrono::high_resolution_clock::now();
	cout << "Result = " << std::setprecision(18) << res << endl;
	cout << "Time: " << (end - start) / std::chrono::milliseconds(1) << "ms" << endl;
	return 0;
}
