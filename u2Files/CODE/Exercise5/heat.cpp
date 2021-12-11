/*************************************************************************
** Computation of heat conduction in a metal plate
** (Iterative solution of a boundary value problem)
** 
** Compile:  g++ -fopenmp -o heat heat.c solver.c
**           g++ -fopenmp -O -o heat heat.c solver.c  // with optimization
** Run:      heat <size> [<epsilon>]
**		        <size>      -- Size of matrix
**	           	<epsilon>   -- accuracy parameter
** Author:   RW
** 
*************************************************************************/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <chrono>


/*
** The iterative computation terminates, if each element has changed
** by at most 'eps', as compared to the last iteration.
*/
double eps = 0.001;

/*
** Execute the iterative solver on the n*n matrix 'a'.
*/
extern int solver(double **a, int n);
	

/* Auxiliary Functions ************************************************* */

/*
** Allocate a 2D array with m * n elements (m rows with n columns).
** I.e., something like ``new double[m][n]'' ...
*/
double ** New_Matrix(int m, int n)
{
	double **res;
	double *array;
	int i;

	/*
	** Result is a 1D array with pointers to the rows of the 2D array
	*/
	res = new double*[m];

	/*
    ** The actual array is allocated in one piece for efficiency reasons.
	** This corresponds to the memory layout that it would have with static
    ** allocation.
	*/
	array = new double[m*n];

	/*
	** Each row pointer is now initialized with the correct value.
	** (Row i starts at element i*n)
	*/
	for (i = 0; i < m; i++) {
		res[i] = &array[i * n];
	}

	return res;
}

/*
** Deallocate a 2D array.
*/
void Delete_Matrix(double **matrix)
{
	delete[] matrix[0];
	delete[] matrix;
}

/*
** Auxiliary function: Print an element of a 2D array.
*/
void print(double **a, int x, int y)
{
	std::cout << "  a[" << std::setw(4) << x << "][" << std::setw(4) << y << "] = "
		 << std::setw(0) << std::setprecision(18) << a[x][y] << std::endl;
}

/* File output ******************************************************** */

/*
** Write the n*n matrix 'a' into the file 'Matrix.txt'.
*/
void Write_Matrix(double **a, int n)
{
	int i, j;
	/* Open file for writing */
	std::fstream file("Matrix.txt", std::ios::out|std::ios::trunc);
	
	if (!file.is_open()) {
		std::cerr << "Cannot open file 'Matrix.txt' for writing!" << std::endl;
		exit(1);
	}

	/* Write the size of the matrix */
	file << n << "\n\n";

	/* Write the matrix elements into the file, row by row */
	file << std::setprecision(10);
	for (j = 0; j < n; j++) {
		for (i = 0; i < n; i++) {
			file << a[i][j] << std::endl;
		}
		file << std::endl;
	}

	/* Close file */
	file.close();
}

/* *********************************************************************** */

int
main(int argc, char **argv)
{
	int i, j;
	int n;
	double **a;

	if ((argc < 2) || (argc > 3)) {
		std::cerr << "Usage: heat <size> [<epsilon>] !" << std::endl << std::endl
			 << "   <size>      -- Size of matrix" << std::endl
			 << "   <epsilon>   -- accuracy parameter" << std::endl;
		exit(1);
	}

	/*
	** First argument: size of the matrix
	*/
	n = atoi(argv[1]);
	if ((n < 3) || (n > 6000)) {
		std::cerr << "Error: size out of range [3 .. 6000] !" << std::endl;
		exit(1);
	}

	/*
	** Second (optional) argument: "accuracy parameter" eps
	*/
	if (argc >= 3) {
		eps = atof(argv[2]);
	}
	if (eps <= 0) {
		std::cerr <<	"Error: epsilon must be > 0! "
			 <<	"(try values between 0.01 and 0.0000000001)" << std::endl;
		exit(1);
	}

	/*
	** Allocate new matrix
	*/
	a = New_Matrix(n, n);
	if (a == NULL) {
		std::cerr << "Can't allocate matrix !" << std::endl;
		exit(1);
	}
		
	/*
	** Initialize the matrix
	*/
	for (i=0; i<n; i++) {
		for (j=0; j<n; j++) {
			a[i][j] = 0;
		}
	}

	/*
	** Assign the boundary values:
	** The upper left and the lower right corner are cold (value 0),
	** the lower left and the upper right corner are hot (value 1),
	** between the corners, the temperature is changing linearly.
	*/
	for (i=0; i<n; i++) {
		double x = (double)i / (n-1);
		a[i][0]       = x;
		a[n-1-i][n-1] = x;
		a[0][i]       = x;
		a[n-1][n-1-i] = x;
	}

	/*
	** Call the iterative solver on matrix 'a'
	*/
	auto start = std::chrono::high_resolution_clock::now();
	int niter = solver(a, n);
	auto end = std::chrono::high_resolution_clock::now();

	/*
	** Write the matrix into a file
	*/
	if (n <= 1000) {
		Write_Matrix(a, n);
	}

	/*
	** Statistics and some verification values
	*/
	std::cout << "Result: " << niter << " iterations" << std::endl;
	i = n/8;
	print(a, n-1-i, i);
	print(a, n-1-i, i/2);
	print(a, n/2, n/2);
	print(a, i/2, n-1-i);
	print(a, i, n-1-i);

	std::chrono::duration<float> time = end - start;
	std::cout << std::fixed << std::setprecision(3) << "Runtime: " << (end - start)/std::chrono::milliseconds(1) << "ms" << std::endl;
	std::cout << "Performance: " << ((double)niter*(n-2)*(n-2)*4/std::chrono::duration_cast<std::chrono::nanoseconds>(time).count()) << " GFlop/s" << std::endl;
	return 0;
}

