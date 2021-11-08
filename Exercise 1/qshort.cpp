#include <stdio.h>
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
        printf("%d ", arr[i]);
    printf("\n");
}

int main()
{
    int a[] = {10,7,8,9,1,5};
    int a_size = sizeof(a) / sizeof(a[0]);
    quickSort(a, 0, a_size - 1);
    printf("Sorted array: \n");
    printArray(a, a_size);
    return 0;
}