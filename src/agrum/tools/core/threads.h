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
 * @brief Utility functions used for exploiting OMP/STD parallelism
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_THREADS_H
#define GUM_THREADS_H

#include <agrum/agrum.h>
#include <agrum/core/threadsSTD.h>
#include <agrum/core/threadsOMP.h>

namespace gum {

  using gum::GUM_THREADS::getMaxNumberOfThreads;
  using gum::GUM_THREADS::getNumberOfLogicalProcessors;
  
} /* namespace gum */


#endif /* GUM_THREADS_H */

