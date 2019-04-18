
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Wrappers for OpenMP.
 * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_OPEM_MP_THREADS_H
#define GUM_OPEM_MP_THREADS_H

#ifdef _OPENMP
#  include <omp.h>
#endif

namespace gum {

  /**
   * @brief Is OMP active ?
   * @ingroup basicstruct_group
   * @return \c True if OMP has been set at compilation, \c False otherwise.
   */
  bool isOMP();

  /**
   * @brief Set the number of threads to be used.
   * @ingroup basicstruct_group
   *
   * To avoid spare cycles (less then 100% CPU occupied), use more threads than
   * logical processors (x2 is a good all-around value).
   * @param number The number of threads to be used.
   */
  void setNumberOfThreads(unsigned int number);

  /**
   * @brief Returns the maximum number of threads at any time.
   * @ingroup basicstruct_group
   *
   * Call this from anywhere (parallel region or not). By default, it is the
   * number of threads launched in any parallel region.
   *
   * It should return the number of logical processors by default, i.e.
   * omp_get_num_procs(). If setNumberOfThreads(number) was called, it will
   * return the choosen number.
   *
   * @return Returns the maximum number of threads at any time.
   */
  unsigned int getMaxNumberOfThreads();

  /**
   * @brief Get the calling thread id.
   * @ingroup basicstruct_group
   *
   * Call this from a parallel region.
   * @return The calling thread id.
   */
  unsigned int getThreadNumber();

  /**
   * @brief Get the current number of running threads.
   * @ingroup basicstruct_group
   *
   * Call this from a parallel region.
   * @return The current number of running threads.
   */
  unsigned int getNumberOfRunningThreads();

  /**
   * @brief Get the number of logical processors.
   * @ingroup basicstruct_group
   * @return The number of logical processors.
   */
  unsigned int getNumberOfLogicalProcessors();

  /**
   * @brief Set nested parallelism (false bu default).
   * @ingroup basicstruct_group
   *
   * Nested parallelism, i.e. parallel activity within another parallel
   * activity : threads creating more threads.
   *
   * Off by default.
   *
   * @param value \c True if nested parallelism should be activated, \c False
   * otherwise.
   */
  void setNestedParallelism(bool value);

  /**
   * @brief Get nested parallelism status.
   * @ingroup basicstruct_group
   * @return \c True if nested parallelism is enabled, \c False otherwise.
   */
  bool getNestedParallelism();

  /**
   * @brief Set the dynamic threads number (false by default).
   * @ingroup basicstruct_group
   *
   * Automatically adjust the number of running threads within a parallel
   * region.
   *
   * Desactivated by default.
   *
   * @param value \c True if dynamic thread number should be used, \c False
   * otherwise.
   */
  void setDynamicThreadsNumber(bool value);

  /**
   * @brief Get the dynamic thread number adjustment status.
   * @ingroup basicstruct_group
   * @ingroup basicstruct_group
   * @return \c True if dynamic adjustment is enabled, \c False otherwise.
   */
  bool getDynamicThreadsNumber();
}   // namespace gum

// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/core/OMPThreads_inl.h>
#endif /* GUM_NO_INLINE */

#endif   // GUM_OPEM_MP_THREADS_H
