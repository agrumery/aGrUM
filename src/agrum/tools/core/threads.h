/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
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
 * @brief Utility functions used for exploiting OMP/STD parallelism
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_THREADS_H
#define GUM_THREADS_H

#include <agrum/agrum.h>
#include <agrum/tools/core/threadsSTL.h>
#include <agrum/tools/core/threadsOMP.h>

namespace gum {

  /**
   * @brief Returns the maximum number of threads at any time.
   * @ingroup basicstruct_group
   *
   * Call this from anywhere (parallel region or not). By default, it is the
   * number of threads launched in any parallel region.
   *
   * @return Returns the maximum number of threads at any time.
   */
  using gum::GUM_THREADS::getMaxNumberOfThreads;

   /**
   * @brief Get the number of logical processors.
   * @ingroup basicstruct_group
   * @return The number of logical processors.
   */
  using gum::GUM_THREADS::getNumberOfLogicalProcessors;

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
   * @brief returns the number of threads used by default when entering the next
   * parallel region
   * @return the number of threads used by default the next time we enter into
   * a parallel region
   */
  unsigned int getCurrentNumberOfThreads();

  /** returns a vector equally splitting elements of a range among threads
   * @brief
   * @param beg the beginning of the range (included)
   * @param end the end of the range (excluded)
   * @param nb_threads the number of threads over which we would like performing
   * the splitting
   * @return a vector containing the range [beginning,end) that each thread should
   * work on
   */
  std::vector< std::pair< Idx, Idx > >
     dispatchRangeToThreads(Idx beg, Idx end, unsigned int nb_threads);
  
} /* namespace gum */


#endif /* GUM_THREADS_H */

