/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief C++17 STL threads convenience utilities for agrum.
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

// to ease automatic parsers
#include <agrum/agrum.h>

#include <agrum/tools/core/threads/threadsSTL.h>

// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#  include <agrum/tools/core/threads/threadsSTL_inl.h>
#endif /* GUM_NO_INLINE */


namespace gum {

  namespace threadsSTL {

    // the number of threads used the next time we enter into a parallel region
    static unsigned int _nb_threads_{getMaxNumberOfThreads()};

    // returns the max number of threads used by default when entering the
    // next parallel region
    unsigned int getNumberOfThreads() { return _nb_threads_; }

    // Set the max number of threads to be used.
    void setNumberOfThreads(unsigned int number) {
      if (number == 0) number = 1;
      _nb_threads_ = number;
    }

  }   // namespace threadsSTL

}   // namespace gum
