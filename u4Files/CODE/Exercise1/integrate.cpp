/***************************************************************************
 *  Numerical Integration                                                  *
 ***************************************************************************/

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <math.h>
#include <chrono>
#include <mpi.h>

constexpr double PI = 3.1415926535897932384626433832795029;

/*
** Globale MPI variable
*/
int myrank, nprocs;


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
	for (i = 0; i < 1000; i++)
		res += 0.004 / (1.0 + (a * a));
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

	h = 1.0 / (double)n;
	sum = 0.0;

	for (i = 1; i <= n; i++) {
		x = h * ((double)i - 0.5);
		sum += f(x);
	}

	return h * sum;
}

/*
** Parallel integration.
**
** Parallelize this function using an MPI reduction
** operation (MPI_Reduce)!
*/
double Parallel_Integration(int n)
{
	int i;
	double h, sum, x;

	h = 1.0 / (double)n;
	sum = 0.0;

	for (i = 1; i <= n; i++) {
		x = h * ((double)i - 0.5);
		sum += f(x);
	}

	return h * sum;
}

int main(int argc, char* argv[])
{
	int n = 0;
	double res;
	int namelen;
	char name[MPI_MAX_PROCESSOR_NAME];

	/* Initialize MPI and set arguments */
	MPI_Init(&argc, &argv);

	/* Determine the number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

	/* Determine own rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	/* Determine node name */
	MPI_Get_processor_name(name, &namelen);

	/* flush results in the output being printed immediately */
	std::cout << "Process " << myrank << "/" << nprocs << "started on " << name << std::endl;


	/*
	** Starting from here, you have to parallelize on your own!
	*/
	if (myrank == 0) {
		for (;;) {
			std::cout << "Enter the number of intervals: (0=exit) " << std::endl;
			std::cin >> n;
			if (n <= 0)
				break;

			// serial solution
			std::cout << "Serial solution:" << std::endl;
			auto t1 = std::chrono::high_resolution_clock::now();
			res = Serial_Integration(n);
			std::chrono::duration<double> t_s = std::chrono::high_resolution_clock::now() - t1;

			std::cout << "  n: " << n << ", integral is approximatly: " << std::setprecision(17) << res << std::endl
				<< "  Error is: " << fabs(res - PI) << ", Runtime[sec]: " << std::setprecision(6) << t_s / std::chrono::seconds(1) << std::endl;

			// parallel solution
			std::cout << "Parallel solution:" << std::endl;
			t1 = std::chrono::high_resolution_clock::now();
			res = Parallel_Integration(n);
			std::chrono::duration<double> t_p = std::chrono::high_resolution_clock::now() - t1;

			std::cout << "  n: " << n << ", integral is approximatly: " << std::setprecision(17) << res << std::endl
				<< "  Error is: " << fabs(res - PI) << ", Runtime[sec]: " << std::setprecision(6) << t_p / std::chrono::seconds(1) << std::endl;
			std::cout << "  Nprocs: " << nprocs
				<< "  Speedup: " << std::setprecision(3)
				<< ((t_p > std::chrono::nanoseconds(0)) ? t_s / t_p : 0.0) << std::endl;
		}
	}
	else {
		/* ... */
	}

	MPI_Finalize();
	return 0;
}



