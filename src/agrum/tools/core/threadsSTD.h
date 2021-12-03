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
 * @brief C++14 threads convenience utilities for agrum.
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_THREADS_STD_H
#define GUM_THREADS_STD_H


#include <thread>
#include <agrum/agrum.h>


namespace gum {

  namespace threadsSTD {

    /**
     * @brief Returns the maximum number of threads at any time.
     * @ingroup basicstruct_group
     *
     * Call this from anywhere (parallel region or not). By default, it is the
     * number of threads launched in any parallel region.
     *
     * @return Returns the maximum number of threads at any time.
     */
    unsigned int getMaxNumberOfThreads();
     
    /**
     * @brief Get the number of logical processors.
     * @ingroup basicstruct_group
     * @return The number of logical processors.
     */
    unsigned int getNumberOfLogicalProcessors();
    

  }   // namespace threadsSTD

} /* namespace gum */


// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/core/threadsSTD_inl.h>
#endif /* GUM_NO_INLINE */


#endif /* GUM_THREADS_STD */
