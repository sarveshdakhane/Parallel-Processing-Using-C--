#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>

using namespace std;

int main()
{  
    int N=50000;
    int i;
    double a[N], b[N], c[N];

    cout<<"Hello JI : ";
    auto start = std::chrono::high_resolution_clock::now();
    //#pragma omp parallel for 
    for (i = 0; i < N; i++)
    {  
       a[i]= b[i]+ c[i];
    }
    auto end = std::chrono::high_resolution_clock::now();
    cout << "Time: " << (end - start) / std::chrono::milliseconds(1) << "ms" << endl;


  /*
    cout<<"Sequential"<<endl;
    auto start1 = std::chrono::high_resolution_clock::now();
    for (i = 0; i < 50000; i++)
    {  
       int m =rand();
       sarr[i]= i*2.5 + m - 3456;
    }
    auto end1 = std::chrono::high_resolution_clock::now();
    cout << "Time: " << (end1 - start1) / std::chrono::milliseconds(1) << "ms" << endl;
  */

    return 0;
}
