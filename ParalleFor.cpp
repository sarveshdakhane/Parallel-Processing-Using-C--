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
    int N=10;
    int x=768;
    double complex;
    int i=5;
    int count=0;
    omp_set_num_threads(2);
 
    #pragma omp parallel for schedule(static,6)   //reduction(+: count)
    for (i=0; i <N; i++)
    {  
       //count+=1;
       complex = (0.0000054 * 0.99999)+90.3333;
       x=888;


       cout<<"Thread :"<<omp_get_thread_num()<<" i: "<<i<<" X:"<<x<<endl;
    }
    cout<<"count is :"<<count;
    return 0;
}
