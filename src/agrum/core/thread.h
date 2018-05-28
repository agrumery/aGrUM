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
 * @brief C++11 threads convenience utilities for agrum.
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_THREAD_H
#define GUM_THREAD_H


#include <thread>


namespace gum {

  namespace thread {

    /**
     * @brief returns the maximum number of threads possible
     * @ingroup basicstruct_group
     *
     * @return Returns the number of concurrent threads supported by the
     * implementation. The value should be considered only a hint.
     */
    unsigned int getMaxNumberOfThreads();

  }   // namespace thread

} /* namespace gum */


// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/core/thread_inl.h>
#endif /* GUM_NO_INLINE */


#endif /* GUM_THREADS */
