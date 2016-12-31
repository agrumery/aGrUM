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
 * @brief OpenMP wrappers implementation.
 * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN
 */

// to ease automatic parsers
#include <agrum/config.h>
#include <agrum/core/OMPThreads.h>

namespace gum {

  INLINE bool isOMP() {
#ifdef _OPENMP
    return true;
#else
    return false;
#endif
  }

  INLINE void setNumberOfThreads( unsigned int number ) {
#ifdef _OPENMP
    omp_set_num_threads( number );
#else
    GUM_ERROR( OperationNotAllowed,
               "openMP was not enabled at compilation (or "
               "you asked for 0 threads !)" );
#endif
  }

  INLINE unsigned int getMaxNumberOfThreads() {
#ifdef _OPENMP
    return omp_get_max_threads();
#else
    return 1;
#endif
  }

  INLINE unsigned int getThreadNumber() {
#ifdef _OPENMP
    return omp_get_thread_num();
#else
    return 0;
#endif
  }

  INLINE unsigned int getNumberOfRunningThreads() {
#ifdef _OPENMP
    return omp_get_num_threads();
#else
    return 1;
#endif
  }

  INLINE unsigned int getNumberOfLogicalProcessors() {
#ifdef _OPENMP
    return omp_get_num_procs();
#else
    return 1;
#endif
  }

  INLINE void setNestedParallelism( bool value ) {
#ifdef _OPENMP
    omp_set_nested( ( ( value == true ) ? 1 : 0 ) );
#else
    GUM_ERROR( OperationNotAllowed,
               "openMP was not enabled at compilation (and you "
               "asked for nested parallelism !)" );
#endif
  }

  INLINE bool getNestedParallelism() {
#ifdef _OPENMP
    return ( ( omp_get_nested() == 0 ) ? false : true );
#else
    return false;
#endif
  }

  INLINE void setDynamicThreadsNumber( bool value ) {
#ifdef _OPENMP
    omp_set_dynamic( ( ( value == true ) ? 1 : 0 ) );
#else
    GUM_ERROR( OperationNotAllowed,
               "openMP was not enabled at compilation (and you "
               "asked for dynamic adjustment of the number of "
               "threads !)" );
#endif
  }

  INLINE bool getDynamicThreadsNumber() {
#ifdef _OPENMP
    return ( ( omp_get_dynamic() == 0 ) ? false : true );
#else
    return false;
#endif
  }

}  // namespace gum
