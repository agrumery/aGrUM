#ifndef ___OMPTHREADS___H___
#define ___OMPTHREADS___H___

#ifdef _OPENMP
  #include <omp.h>
#endif

namespace gum_threads {

  inline bool isOMP () {
    #ifdef _OPENMP
      return true;
    #else
      return false;
    #endif
  }

  // to avoid spare cycles (less than 100% CPUs occupied), use more threads than logical processors (x2)
  inline void setNumberOfThreads ( unsigned int number ) {
    #ifdef _OPENMP
      omp_set_num_threads ( number );
    #else
      GUM_ERROR ( gum::OperationNotAllowed, "openMP was not enabled at compilation (or you asked for 0 threads !)" );
    #endif
  }

  // call this from anywhere (parallel region or not)
  // this is the default number of threads in any parallel region
  // SHOULD return the number of logical processors by default
  // ( omp_get_num_procs() ) and will return x if 
  // setNumberOfThreads(x) was called
  inline unsigned int getMaxNumberOfThreads () {
    #ifdef _OPENMP
      return omp_get_max_threads ();
    #else
      return 1;
    #endif
  }

  // instead of using #define omp_get_thread_num () 0 if !_OPENMP
  // always use this functions instead (no omp functions in agrum !)

  // call this from a parallel region (from a thread)
  inline unsigned int getThreadNumber () {
    #ifdef _OPENMP
      return omp_get_thread_num ();
    #else
      return 0;
    #endif
  }

  // call this from a parallel region (from a thread)
  inline unsigned int getNumberOfRunningThreads () {
    #ifdef _OPENMP
      return omp_get_num_threads (); 
    #else
      return 1;
    #endif
  }

  // get number of processors
  inline unsigned int getNumberOfLogicalProcessors () {
    #ifdef _OPENMP
      return omp_get_num_procs ();
    #else
      return 1;
    #endif
  }

  // nested parallelism i.e. parallel for within another parallel for
  // threads creating more threads (off by default)
  inline void setNestedParallelism ( bool value ) {
    #ifdef _OPENMP
      omp_set_nested ( ( ( value == true ) ? 1 : 0 ) );
    #else
      GUM_ERROR ( gum::OperationNotAllowed, "openMP was not enabled at compilation (and you asked for nested parallelism !)" );
    #endif
  }

  inline bool getNestedParallelism () {
    #ifdef _OPENMP
      return ( ( omp_get_nested () == 0 ) ? false : true );
    #else
      return false;
    #endif
  }

  // auto adjustment of the number of threads in a parallel region
  // off by default
  inline void setDynamicThreadsNumber ( bool value ) {
    #ifdef _OPENMP
      omp_set_dynamic ( ( ( value == true ) ? 1 : 0 ) );
    #else
      GUM_ERROR ( gum::OperationNotAllowed, "openMP was not enabled at compilation (and you asked for dynamic adjustment of the number of threads !)" );
    #endif
  }

  inline bool getDynamicThreadsNumber () {
    #ifdef _OPENMP
      return ( ( omp_get_dynamic () == 0 ) ? false : true );
    #else
      return false;
    #endif
  }

}

#endif
