/*************************************************************************
** Implementation of the quicksort algorithm.
** 
** Compile:  gcc -pthread -Wall -o qsort qsort.c
**           gcc -pthread -Wall -O -o qsort qsort.c   // with optimization
** Run:      qsort <size-of-array>
** Author:   AK  
** 
*************************************************************************/

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <chrono>



/*
** Quicksort: sort the elements 'lo' ... 'hi' in array 'a'.
*/
void quicksort(int *a, int lo, int hi)
{
	int i, j;
	int pivot;

	if (lo >= hi) {
		/*
		** Sub-array is empty or has only one element,
		** i.e., it is already sorted
		*/
		return;
	}

	/* Determine pivot (center element of the array) */
	pivot = a[(lo+hi)/2];

	/* Split aray a into two parts */
	i = lo;
	j = hi;
	while (i <= j) {    
		/*
		** Search an element < pivot starting from below
		*/
		while (a[i] < pivot)
			i++; 

		/*
		** Search an element > pivot starting from above
		*/
		while (a[j] > pivot)
			j--;

		if (i <= j) {
			/*
			** Swap a[i] and a[j]
			*/
			int tmp = a[i];
			a[i] = a[j];
			a[j] = tmp;

			i++;
			j--;
		}
	}
	
	/* Recursion */
	quicksort(a, lo, j);
	quicksort(a, i, hi);
}

/*
** Initialize an array 'a' of length 'n' with
** random numbers.
*/
void initialize(int *a, int n)
{
	int i;

	/* 
	** Set the start value for the random number generator.
	** Comment this out, if you always want to get the same
	** numbers (for debugging)!
	*/
	srand((unsigned int)std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));

	for (i = 0; i < n; i++) {
		a[i] = rand();
	}
}

/*
** Check if an array 'a' of length 'n' is sorted.
** If yes, the result is 1, otherwise the result is 0.
*/
int checkSorted(int *a, int n)
{
	int i;
	
	for (i = 1; i < n; i++) {
		if (a[i] < a[i-1])
			return 0;
	}
	return 1;
}

/*
** Print an array 'a' of length 'n'.
*/
void printArray(int *a, int n)
{
	int i;
	
	for (i = 0; i < n; i++) {
		std::cout << a[i] << " ";
	}
	std::cout << std::endl;
}


/*
** Main program.
** Invocation: qsort <array length>
*/
int main(int argc, char **argv)
{
	int *array;
	int n;

	if (argc < 2) {
		std::cerr << "Usage: qsort <array-size>" << std::endl;
		exit(1);
	}

	/*
	** 'n' is the command line parameter,
	** converted into an integer number.
	*/
	n = atoi(argv[1]);
	if ((n <= 0) || (n > 200000000)) {
		std::cerr << "Illegal array size!" << std::endl;
		exit(2);
	}

	/*
	** Allocate the array
	*/
	array = new int[n];

	/*
	** Initialisze the array
	*/
	initialize(array, n);

	/*
	** Sort and measure the time ...
	*/
	auto timeStart = std::chrono::high_resolution_clock::now();
	quicksort(array, 0, n-1);
	auto timeEnd = std::chrono::high_resolution_clock::now();

	/*
	** Check it the array is sorted correctly.
	*/
	if (checkSorted(array, n)) {
		std::cout << "OK, array is sorted!" << std::endl;
	}
	else {
		std::cout << "OOPS, array is NOT sorted!" << std::endl;
	}

	std::cout << "Time for sorting: " << std::setprecision(3) << (timeEnd - timeStart)/std::chrono::milliseconds(1) << "ms" << std::endl;
	
	return 0;
}
