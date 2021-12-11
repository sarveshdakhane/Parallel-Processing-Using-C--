/***************************************************************************
 *  Numerical Integration                                                  *
 ***************************************************************************/

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <chrono>


constexpr double PI = 3.1415926535897932384626433832795029;


/*
** This function is integrated over the interval [0, 1]. The value of the
** integral is exactly PI (for purposes of comparison).
** 
** The function is deliberately written extremely inefficiently in order to
** make the problem more practical: in the case of numerical integration, the
** main effort is usually the calculation of the function, not the summation.
*/
double f(double a) 
{
    double res = 0;
    int i;
    for (i=0; i<1000; i++)
        res += 0.004/(1.0+(a*a));
    return res;
}


/*
** Sequential integration for purposes of comparison.
** Please do not modify this code!
*/
double Serial_Integration(int n)
{
	int i;
	double h, sum, x;
    
	h = 1.0/(double)n; 
	sum = 0.0;
    
	for (i=1; i<=n; i++) {
		x = h * ((double)i - 0.5);
		sum += f(x);
	}
    
	return h * sum;
}

/*
** Parallel integration.
**
** Parallelize this function using a reduction!
*/
double Parallel_Integration1(int n)
{
	int i;
	double h, sum, x;
    
	h = 1.0/(double)n; 
	sum = 0.0;

	for (i=1; i<=n; i++) {
		x = h * ((double)i - 0.5);
		sum += f(x);
	}
	
	return h * sum;   
}

/*
** Parallel integration.
**
** Parallelize this function using the OpenMP ordered
** directive, such that the result is exactly the same
** as the result of Serial_Integration()!
*/
double Parallel_Integration2(int n)
{
	int i;
	double h, sum, x;
    
	h = 1.0/(double)n; 
	sum = 0.0;

	for (i=1; i<=n; i++) {
		x = h * ((double)i - 0.5);
		sum += f(x);
	}
	
	return h * sum;   
}

/*
** Parallel integration.
**
** Parallelize this function, such that the result is exactly
** the same as the result of Serial_Integration()!
** To do so, split the loop into a parallel and a sequential
** part!
**
** Note: In C++, you can create a 'double' array of length n using the
** statement:
**    double *a = new double[n];
** Do not forget to deallocate the array at the end:
**    delete[] a;
*/
double Parallel_Integration3(int n)
{
	int i;
	double h, sum, x;
    
	h = 1.0/(double)n; 
	sum = 0.0;

	for (i=1; i<=n; i++) {
		x = h * ((double)i - 0.5);
		sum += f(x);
	}
	
	return h * sum;   
}

/*
** For simplicity: this function performs the integration using
** the function 'integr', returns the runtime and prints the
** obtained speedup, if applicable.
*/
std::chrono::nanoseconds execute(double (*integr)(int), int n, std::chrono::nanoseconds t_s) 
{
	double res;
	int nthreads;

	auto t1 = std::chrono::high_resolution_clock::now();
	res = integr(n);     
	auto t_p = std::chrono::high_resolution_clock::now() - t1;
	
    
	std::cout << "  n: " << n << ", integral is approximatly: " << std::setprecision(17) << res << std::endl
		 << "  Error is: " << fabs(res-PI) << ", Runtime[ms]: " << std::setprecision(6) << t_p/std::chrono::milliseconds(1) << std::endl;
	if (t_s > std::chrono::nanoseconds(0)) {
		nthreads = omp_get_max_threads();
		std::cout << "  Nthreads: " << nthreads
			 << "  Speedup: " << std::setprecision(3) << ((double)t_s.count()/t_p.count()) << std::endl;
	}
	std::cout << std::endl;

	return t_p;
}

int main(int argc, char *argv[])
{
	int n=0;
	
  
	for (;;) {
		std::cout << "Enter the number of intervals: (0=exit) " << std::endl;
		std::cin >> n;
		if (n <= 0)
			break;

		/* warm-up ... */
		Serial_Integration(n);
		
		/* serial solution */
		std::cout << "Serial solution:" << std::endl;;
		auto t_s = execute(Serial_Integration, n, std::chrono::nanoseconds(0));
    
		/* parallel solution 1 */
		std::cout << "Parallel solution 1:" << std::endl;
		execute(Parallel_Integration1, n, t_s);
    
		/* parallel solution 2 */
		std::cout << "Parallel solution 2:" << std::endl;
		execute(Parallel_Integration2, n, t_s);
    
		/* parallel solution 3 */
		std::cout << "Parallel solution 3:" << std::endl;
		execute(Parallel_Integration3, n, t_s);
	}
    
	return 0;
}

