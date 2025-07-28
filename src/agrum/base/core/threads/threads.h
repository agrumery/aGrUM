/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Utility functions used for exploiting OpenMP/STL parallelism
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_THREADS_H
#define GUM_THREADS_H

#include <utility>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/base/core/threads/threadsOMP.h>
#include <agrum/base/core/threads/threadsSTL.h>

namespace gum {

  // select the default threading model
  // @TODO substitute the lines below by namespace threads = GUM_THREADS; when swig
  // will support it
#ifdef GUM_THREADS_USE_OMP
  namespace threads = gum::threadsOMP;
#else
  namespace threads = gum::threadsSTL;
#endif


  /**
   * @brief Returns the maximum number of threads you should use at any time.
   * @ingroup basicstruct_group
   *
   * By default, it should be the number of CPU cores available. Note that it
   * is preferable to use function getNumberOfThreads(), which corresponds to the
   * max number of threads the user wants to launch in parallel regions (this
   * is by default equal to getMaxNumberOfThreads(), but the user may wish to
   * change it for some reason).
   *
   * @return Returns the maximum number of threads that should be used at any time.
   */
  using gum::threads::getMaxNumberOfThreads;

  /**
   * @brief returns the max number of threads used by default when entering the next
   * parallel region
   *
   * This is the number of threads launched in parallel regions. By default, this
   * number is equal to getMaxNumberOfThreads() but if the user has
   * changed it using method setNumberOfThreads, then this number is what the
   * user required.
   *
   * @return the number of threads used by default the next time we enter into
   * a parallel region
   */
  using gum::threads::getNumberOfThreads;

  /**
   * @brief Get the number of logical processors.
   * @ingroup basicstruct_group
   * @return The number of logical processors.
   */
  using gum::threads::getNumberOfLogicalProcessors;

  /**
   * @brief Set the max number of threads to be used when entering the next
   * parallel region.
   * @ingroup basicstruct_group
   *
   * To avoid spare cycles (less then 100% CPU occupied), you may want to use more
   * threads than logical processors (x2 could be a good all-around value).
   * @param number The number of threads to be used in the next parallel region.
   */
  void setNumberOfThreads(unsigned int number);

  /// indicates whether aGrUM uses openMP or STL threads
  bool isOMP();

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
