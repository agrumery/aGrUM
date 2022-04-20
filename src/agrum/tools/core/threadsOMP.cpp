/**
 *
 *   Copyright (c) 2005-2022 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief OpenMP wrapper implementation.
 *
 * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN
 */
#include <agrum/agrum.h>

#include <agrum/tools/core/threadsOMP.h>

// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#  include <agrum/tools/core/threadsOMP_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {

  namespace threadsOMP {

    // the number of threads used the next time we enter into a parallel region
    static unsigned int _nb_threads_{getMaxNumberOfThreads()};


    // returns the max number of threads used by default when entering the
    // next parallel region
    unsigned int getNumberOfThreads() { return _nb_threads_; }


    // Set the max number of threads to be used.
    void setNumberOfThreads(unsigned int number) {
      if (number == 0) number = 1;
#ifdef _OPENMP
      omp_set_num_threads(number);
      _nb_threads_ = number;
#else
      if (number != 1) {
        GUM_ERROR(OperationNotAllowed,
                  "openMP was not enabled at compilation (or "
                  "you asked for 0 threads !)");
      }
#endif
    }

  }   // namespace threadsOMP

}   // namespace gum
