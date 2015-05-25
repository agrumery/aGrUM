/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN and Christophe GONZALES  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/**
 * @file
 * @brief wrapper for OMP
 * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN
 */

#ifndef __OMPTHREADS__H__
#define __OMPTHREADS__H__

#ifdef _OPENMP
#include <omp.h>
#endif

namespace gum {

  /**
   * Is OMP active ?
   * @return \c True if OMP has been set at compilation, \c False otherwise.
   */
  inline bool isOMP() {
#ifdef _OPENMP
    return true;
#else
    return false;
#endif
  }

  /**
   * Set the number of threads to be used.
   *
   * To avoid spare cycles (less then 100% CPU occupied), use more threads than
   *logical processors (x2 is a good all-around value).
   * @param number The number of threads to be used.
   */
  inline void setNumberOfThreads(unsigned int number) {
#ifdef _OPENMP
    omp_set_num_threads(number);
#else
    GUM_ERROR(
        OperationNotAllowed,
        "openMP was not enabled at compilation (or you asked for 0 threads !)");
#endif
  }

  /**
   * Call this from anywhere (parallel region or not). By default, it is the number
   *of threads launched in any parallel region.
   *
   * It should return the number of logical processors by default, i.e.
   *omp_get_num_procs(). If setNumberOfThreads(number) was called, it will return the
   *choosen number.
   *
   * @return The maximum number of threads at any time.
   */
  inline unsigned int getMaxNumberOfThreads() {
#ifdef _OPENMP
    return omp_get_max_threads();
#else
    return 1;
#endif
  }

  /**
   * Get the calling thread id.
   *
   * Call this from a parallel region.
   * @return The calling thread id.
   */
  inline unsigned int getThreadNumber() {
#ifdef _OPENMP
    return omp_get_thread_num();
#else
    return 0;
#endif
  }

  /**
   * Get the current number of running threads.
   *
   * Call this from a parallel region.
   * @return The current number of running threads.
   */
  inline unsigned int getNumberOfRunningThreads() {
#ifdef _OPENMP
    return omp_get_num_threads();
#else
    return 1;
#endif
  }

  /**
   * Get the number of logical processors.
   * @return The number of logical processors.
   */
  inline unsigned int getNumberOfLogicalProcessors() {
#ifdef _OPENMP
    return omp_get_num_procs();
#else
    return 1;
#endif
  }

  /**
   * Nested parallelism, i.e. parallel activity within another parallel activity :
   * threads creating more threads. Off by default.
   * @param value \c True if nested parallelism should be activated, \c False
   * otherwise.
   */
  inline void setNestedParallelism(bool value) {
#ifdef _OPENMP
    omp_set_nested(((value == true) ? 1 : 0));
#else
    GUM_ERROR(OperationNotAllowed, "openMP was not enabled at compilation (and you "
                                   "asked for nested parallelism !)");
#endif
  }

  /**
   * Get nested parallelism status.
   * @return \c True if nested parallelism is enabled, \c False otherwise.
   */
  inline bool getNestedParallelism() {
#ifdef _OPENMP
    return ((omp_get_nested() == 0) ? false : true);
#else
    return false;
#endif
  }

  /**
   * Automatically adjust the number of running threads within a parallel region.
   * Desactivated by default.
   * @param value \c True if dynamic thread number should be used, \c False
   * otherwise.
   */
  inline void setDynamicThreadsNumber(bool value) {
#ifdef _OPENMP
    omp_set_dynamic(((value == true) ? 1 : 0));
#else
    GUM_ERROR(OperationNotAllowed, "openMP was not enabled at compilation (and you "
                                   "asked for dynamic adjustment of the number of "
                                   "threads !)");
#endif
  }

  /**
   * Get the dynamic thread number adjustment status.
   * @return \c True if dynamic adjustment is enabled, \c False otherwise.
   */
  inline bool getDynamicThreadsNumber() {
#ifdef _OPENMP
    return ((omp_get_dynamic() == 0) ? false : true);
#else
    return false;
#endif
  }
}

#endif
