#include <iostream>
#include <iomanip>
#include <math.h>
#include <chrono>
#include <thread>
#include <future>
#include <random>

using namespace std;

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
        {i++;}
        while (j >= start && a[j] >= a[pivot])
        {j--;}
        if (i <= j)
        {        
            swap(&a[i], &a[j]);
        }
    }
    swap(&a[i], &a[pivot]);
    if(start < i-1)
    {

        quickSort(a,start,i-1);
    }
    if(i+1<end)
    {
        quickSort(a,i+1,end);
    }
}

void printArray(int arr[], int size)
{
    int i;
    for (i = 0; i < size; i++)
    {
        cout << " "<<arr[i];
    }
    cout << endl;
}



void initialize(int *a, int n)
{
    for(int i=0;i<n;i++)
    {
      cout<<"Elements no "<<i+1<<"::"<<a[i]<<endl;
    }
}


int main()
{   try{
    int randomNumber = rand();
    cout << "Run Started " << randomNumber % 100;
    int ArrayLength= 10000000;
    int a[ArrayLength];
    for(int i=0;i<ArrayLength;i++)
    {
       a[i]= rand() % 10;
    }
    //initialize(&a[ArrayLength], ArrayLength);
    cout << "Array Genereted: " ;
    auto start = std::chrono::high_resolution_clock::now();
    quickSort(a, 0, ArrayLength - 1);
    auto end = std::chrono::high_resolution_clock::now();
	cout << "Time: " << (end - start)/std::chrono::milliseconds(1) << "ms" << endl;
    cout << "Sorted array:" ;
    printArray(a, ArrayLength);
    }
    catch(std::exception& e)
    {
            std::cout << e.what();
    }
    return 0;
}