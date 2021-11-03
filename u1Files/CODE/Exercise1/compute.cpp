#include <iostream>
#include <iomanip>
#include <math.h>
#include <chrono>

extern double f1();
extern double f2(double a);
extern double f3(double a);
extern double f4(double a);
extern double f5(double a);
extern double f6(double a, double b);
extern double f7(double a);

int main(int argc, char **argv)
{
	double a, b, c, res;
	auto start = std::chrono::high_resolution_clock::now();

	a = f1();
    b = f2(a);
	c = f3(a);
	a = f4(a);
	res = f5(b) + f7(a) + f6(b,c);
	auto end = std::chrono::high_resolution_clock::now();
	
	std::cout << "Result = " << std::setprecision(18) << res << std::endl;
	std::cout << "Time: " << (end - start)/std::chrono::milliseconds(1) << "ms" << std::endl;
	return 0;
}
