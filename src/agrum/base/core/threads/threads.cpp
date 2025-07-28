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

#include <agrum/agrum.h>

#include <agrum/base/core/threads/threads.h>

namespace gum {

  // Set the number of threads to be used in the next parallel region
  void setNumberOfThreads(unsigned int number) {
    if (number == 0) number = 1;

    threadsSTL::setNumberOfThreads(number);

#ifdef _OPENMP
    threadsOMP::setNumberOfThreads(number);
#endif
  }

  // indicates whether aGrUM uses openMP or STL threads
  bool isOMP() {
#ifdef GUM_THREADS_USE_OMP
    return true;
#else
    return false;
#endif
  }

  // returns a vector equally splitting elements of a range among threads
  std::vector< std::pair< Idx, Idx > >
      dispatchRangeToThreads(Idx beg, Idx end, unsigned int nb_threads) {
    // there should always be at least one thread
    const auto real_nb_threads = nb_threads < 1 ? Idx(1) : Idx(nb_threads);
    const Idx  range_size      = end - beg;
    std::vector< std::pair< Idx, Idx > > result;
    result.reserve(real_nb_threads);

    // if the number of elements in the range is lower than or equal to the
    // number of threads, assign only one element per thread
    if (range_size <= real_nb_threads) {
      for (Idx i = Idx(0); i < range_size; ++i) {
        result.emplace_back(i, i + 1);
      }
      for (Idx i = range_size; i < real_nb_threads; ++i)
        result.emplace_back(range_size, range_size);
    } else {
      // here there are more elements in the range than threads. So try to balance
      // the number of elements among the threads
      Idx nb_elts_par_thread = range_size / real_nb_threads;
      Idx rest_elts          = range_size - nb_elts_par_thread * real_nb_threads;

      Idx begin_index = beg;
      for (Idx i = Idx(0); i < real_nb_threads; ++i) {
        Idx end_index = begin_index + nb_elts_par_thread;
        if (rest_elts != Idx(0)) {
          ++end_index;
          --rest_elts;
        }
        result.emplace_back(begin_index, end_index);
        begin_index = end_index;
      }
    }

    return result;
  }

}   // namespace gum
