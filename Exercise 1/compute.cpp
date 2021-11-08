#include <iostream>
#include <iomanip>
#include <math.h>
#include <chrono>
#include <thread>
#include <future>

using namespace std;

extern double f1();
extern double f2(double a);
extern double f3(double a);
extern double f4(double a);
extern double f5(double a);
extern double f6(double a, double b);
extern double f7(double a);

double f1()
{
	cout<<"Called F1 "<<endl;
	return 9007190000;
}
double f2(double a)
{
	cout<<"Called F2 "<<endl;

	double Oddsum=0;
	for (int i=0; i<=507190000; ++i)
	{
		if((i&1)==0)
		{
			Oddsum +=i;

		}
	}
	//b= a * Oddsum;
	return a * Oddsum;
}
double f3(double a)
{
	cout<<"Called F3 "<<endl;
	double Evensum=0;
	for (int i=0; i<=20070000; ++i)
	{
		if((i&1)==0)
		{
			Evensum +=i;
     	}
	}
	return a * Evensum;
}
double f4(double a)
{
	cout<<"Called F4"<<endl ;
	//a1=a+9009925;
	return a+9009925000;
}
double f5(double a)
{
	cout<<"Called F5"<<endl ;
	return a*19222540000;
}
double f6(double a,double b)
{
	cout<<"Called F6"<<endl ;
	return a*b;
}
double f7(double a)
{
	cout<<"Called F7"<<endl;
	return a +19007193425;
}


int main(int argc, char **argv)
{
	cout<<"Hello Ji..."<<endl;
	
	double a, a1,b, c, res;
	auto start = std::chrono::high_resolution_clock::now();
	
	///*

    a = f1();
	auto Future_f2=std::async(f2,a);
	auto Future_f3=std::async(f3,a);
	b=Future_f2.get();
	c=Future_f3.get();	
    auto Future_f4=std::async(f4,a);
    a1 = Future_f4.get(); 
    auto Future_f5 = std::async(f5,b) ;
	auto Future_f7 = std::async(f7,a1);
	auto Future_f6 = std::async(f6,b,c);
    res = Future_f5.get() + Future_f7.get() + Future_f6.get();
	//*/

   /*
    a = f1();
	std::thread t2(f2,a);
	std::thread t3(f3,a);
    std::thread t4(f4,a);
	t2.join();
	t3.join();
	t4.join();
    res = f5(b) + f7(a) + f6(b,c);
	*/
	
    
	/*
	a = f1();
    b = f2(a);	
	c = f3(a);
	a = f4(a);
	res = f5(b) + f7(a) + f6(b,c);
	*/
    
	auto end = std::chrono::high_resolution_clock::now();
	
	cout << "Result = " << std::setprecision(18) << res << endl;
	cout << "Time: " << (end - start)/std::chrono::milliseconds(1) << "ms" << endl;
	return 0;
}
