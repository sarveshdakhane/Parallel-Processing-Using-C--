#include <iostream>
#include <iomanip>
#include <math.h>
#include <chrono>
#include <thread>
#include <future>
#include <random>
#include <omp.h>
#include <utility>
#include <array>

using namespace std;
extern void quickSort(int a[], int start, int end);
extern const int MINSIZE = 1000000;

void swap(int *f, int *s)
{
    try
    {
        int t = *f;
        *f = *s;
        *s = t;
    }
    catch (std::exception &e)
    {
        std::cout << e.what();
    }
}
void quickSort(int a[], int start, int end)
{
    try
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

       // /*
        if (start < i - 1)
        {
            if ((i - 1) - start > MINSIZE)
            {
#pragma omp parallel sections
                {
#pragma omp section { quickSort(a, start, i - 1); }
                }
            }
            else
            {
                quickSort(a, start, i - 1);
            }
        }
        if (i + 1 > end)
        {
#pragma omp parallel sections
            {
#pragma omp section { quickSort(a, i + 1, end); }
            }
        }
      //  */
    /*
    if(start<i-1)
    {      
    quickSort(a, start, i - 1); 
    }
    
    if(i+1>end)
    {
    quickSort(a, i + 1, end);
    }
  */
    }
    catch (std::exception &e)
    {
        std::cout << e.what();
    }
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
bool CheckSorted(int *a, int n)
{
    if (n == 1 || n == 0)
    {
        return 0;
    }
    if (a[n - 1] < a[n - 2])
    {
        return 1;
    }
    return CheckSorted(a, n - 1);
}
int main()
{
    try
    {
        cout << "Run Started...." << endl;
        int ArrayLength = 1000000000, len = 0;
        int *a = new int[ArrayLength];
        initialize(a, ArrayLength);
        cout << "Array Genereted: " << endl;
        //printArray(a, ArrayLength);
        auto start = std::chrono::high_resolution_clock::now();
        quickSort(a, 0, ArrayLength - 1);
        //printArray(a, ArrayLength);
        /*
        if (CheckSorted(a, ArrayLength))
            {cout<<"Yes"<<endl;}
        else
            {cout<<"No"<<endl;}
        */
        auto end = std::chrono::high_resolution_clock::now();
        cout << "Time: " << (end - start) / std::chrono::milliseconds(1) << "ms" << endl;
    }
    catch (std::exception &e)
    {
        std::cout << e.what();
    }
    return 0;
}