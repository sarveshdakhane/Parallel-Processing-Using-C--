#include <iostream>
#include <iomanip>
#include <math.h>
#include <chrono>

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
	cout<<"Called F1 "<<endl ;
	return 1;
}

double f2(double a)
{
	cout<<"Called F2"<<endl ;
	return 1;
}
double f3(double a)
{
	cout<<"Called F3 "<<endl ;
	return 1;
}
double f4(double a)
{
	cout<<"Called F4"<<endl ;
	return 1;
}
double f5(double a)
{
	cout<<"Called F5"<<endl ;
	return 1;
}
double f6(double a,double b)
{
	cout<<"Called F6"<<endl ;
	return 1;
}
double f7(double a)
{
	cout<<"Called F7"<<endl ;
	return 1;
}


int main(int argc, char **argv)
{
	cout<<"Hello Ji..."<<endl;
	
	double a, b, c, res;
	auto start = std::chrono::high_resolution_clock::now();
    
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
