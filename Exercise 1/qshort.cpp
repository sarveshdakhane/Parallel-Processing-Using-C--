#include <iostream>
#include <iomanip>
#include <math.h>
#include <chrono>
#include <thread>
#include <future>
#include <random>
#include <omp.h>

using namespace std;

extern void quickSort(int a[], int start, int end);
extern void RunParallel(int a, int i,int start,int end);

void RunParallel(int a[], int i,int start,int end)
{

    
       #pragma omp parallel sections
            {
        #pragma omp section
                {
                    if (start < i - 1)
                    {
                        quickSort(a, start, i - 1);
                    }
                }
        #pragma omp section
                {
                    if (i + 1 < end)
                    {
                        quickSort(a, i + 1, end);
                    }
                }
            }

}
void Sequencially(int a[], int i,int start,int end)
{

     if (start < i - 1)
                    {
                        quickSort(a, start, i - 1);
                    }
                    if (i + 1 < end)
                    {
                        quickSort(a, i + 1, end);
                    }
}

void swap(int *f, int *s)
{
    int t = *f;
    *f = *s;
    *s = t;
}

void quickSort(int a[], int start, int end)
{
    int i = start, j = end - 1, pivot = end;
    while (i <= j)
    {
        while (a[i] < a[pivot])
        {
            i++;
        }
        while (j >= start && a[j] >= a[pivot])
        {
            j--;
        }
        if (i <= j)
        {
            swap(&a[i], &a[j]);
        }
    }
     swap(&a[i], &a[pivot]);

    //RunParallel(a,i,start,end);
    //cout<<"Parallel";

    Sequencially(a,i,start,end);
    //cout<<"Sequential";

}

void printArray(int arr[], int size)
{
    for (int i = 0; i < size; i++)
    {
        cout << " " << arr[i];
    }
    cout << endl;
}
void initialize(int *a, int n)
{
    for (int i = 0; i < n; i++)
    {
        a[i] = rand() % 100;
    }
}
int main()
{
    try
    {
        cout << "Run Started " << endl;
        int ArrayLength = 1000000;
        int *a = new int[ArrayLength];
        initialize(a, ArrayLength);
        cout << "Array Genereted: " << endl;
        auto start = std::chrono::high_resolution_clock::now();
        quickSort(a, 0, ArrayLength - 1);
        auto end = std::chrono::high_resolution_clock::now();
        cout << "Time: " << (end - start) / std::chrono::milliseconds(1) << "ms" << endl;
        cout << "Sorted array:";
       // printArray(a, ArrayLength);
    }
    catch (std::exception &e)
    {
        std::cout << e.what();
    }
    return 0;
}