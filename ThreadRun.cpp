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

int main () {
  #pragma omp parallel num_threads(3) /// We could pass this value while run time too
  {
    
    /*
    if (omp_get_thread_num() == 0) {
      std::cout << "Parallel Num threads:" << omp_get_num_threads() << std::endl;
    }
    */
#pragma omp parallel
  {
      cout<<"Yes"<<endl;
  }

  cout<< omp_get_num_procs()<<endl;

  };

  




  cout<<"Serials";
}