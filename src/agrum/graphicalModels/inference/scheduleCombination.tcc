/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
/** @file
 * @brief A generic class to combine efficiently several ScheduleMultiDims
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS


#include <agrum/config.h>


namespace gum {

  
  /// constructor
  template<typename T_DATA>
  ScheduleCombination<T_DATA>::ScheduleCombination () {
    /// for debugging purposes
    GUM_CONSTRUCTOR ( ScheduleCombination );
  }

  
  /// copy constructor
  template<typename T_DATA>
  ScheduleCombination<T_DATA>::ScheduleCombination
  ( const ScheduleCombination<T_DATA>& from ) {
    /// for debugging purposes
    GUM_CONS_CPY ( ScheduleCombination );
  }

  
  /// destructor
  template<typename T_DATA>
  ScheduleCombination<T_DATA>::~ScheduleCombination () {
    /// for debugging purposes
    GUM_DESTRUCTOR ( ScheduleCombination );
  }


  // adds to a given schedule the operations necessary to perform a combination
  template<typename T_DATA>
  ScheduleMultiDim<T_DATA>
  ScheduleCombination<T_DATA>::combine
  ( const Set<const MultiDimImplementation<T_DATA>*>& set,
    Schedule<T_DATA>& schedule ) {
    // first wrap the multidimimplementations into ScheduleMultiDims
    Set<const ScheduleMultiDim<T_DATA>*> sched_set;
    for ( typename Set<const MultiDimImplementation<T_DATA>*>::const_iterator
            iter = set.begin(); iter != set.end(); ++iter ) {
      sched_set.insert ( new ScheduleMultiDim<T_DATA> ( **iter ) );
    }

    // perform the combination
    const ScheduleMultiDim<T_DATA>& res = combine ( sched_set, schedule );

    // deallocate the wrappers we just constructed
    for ( typename Set<const ScheduleMultiDim<T_DATA>*>::const_iterator
            iter = sched_set.begin(); iter != sched_set.end(); ++iter ) {
      delete *iter;
    }

    return res;
  }

  
  // adds to a given schedule the operations necessary to perform a combination
  template <typename T_DATA>
  template <template<typename> class TABLE>
  ScheduleMultiDim<T_DATA>
  ScheduleCombination<T_DATA>::combine ( const Set<const TABLE<T_DATA>*>& set,
                                         Schedule<T_DATA>& schedule ) {
    // first wrap the TABLES into ScheduleMultiDims
    Set<const ScheduleMultiDim<T_DATA>*> sched_set;
    for ( typename Set<const TABLE<T_DATA>*>::const_iterator iter = set.begin();
          iter != set.end(); ++iter ) {
      sched_set.insert
        ( new ScheduleMultiDim<T_DATA> ( *( (*iter)->content() ) ) );
    }

    // perform the combination
    const ScheduleMultiDim<T_DATA>& res = combine ( sched_set, schedule );
    
    // deallocate the wrappers we just constructed
    for ( typename Set<const ScheduleMultiDim<T_DATA>*>::const_iterator
            iter = sched_set.begin(); iter != sched_set.end(); ++iter ) {
      delete *iter;
    }

    return res;
  }

  
  /** @brief returns a rough estimate of the number of operations that will be
   * performed to compute the combination */
  template <typename T_DATA>
  float ScheduleCombination<T_DATA>::nbOperations
  ( const Set<const MultiDimImplementation<T_DATA>*>& set,
    const Schedule<T_DATA>& schedule ) {
    // first wrap the multidimimplementations into ScheduleMultiDims
    Set<const ScheduleMultiDim<T_DATA>*> sched_set;
    for ( typename Set<const MultiDimImplementation<T_DATA>*>::const_iterator
            iter = set.begin(); iter != set.end(); ++iter ) {
      sched_set.insert ( new ScheduleMultiDim<T_DATA> ( **iter ) );
    }

    // perform the combination
    float res = nbOperations ( sched_set, schedule );

    // deallocate the wrappers we just constructed
    for ( typename Set<const ScheduleMultiDim<T_DATA>*>::const_iterator
            iter = sched_set.begin(); iter != sched_set.end(); ++iter ) {
      delete *iter;
    }

    return res;
  }

  
  /** @brief returns a rough estimate of the number of operations that will be
   * performed to compute the combination */
  template <typename T_DATA>
  template <template<typename> class TABLE>
  float ScheduleCombination<T_DATA>::nbOperations
  ( const Set<const TABLE<T_DATA>*>& set,
    const Schedule<T_DATA>& schedule ) {
    // first wrap the TABLES into ScheduleMultiDims
    Set<const ScheduleMultiDim<T_DATA>*> sched_set;
    for ( typename Set<const TABLE<T_DATA>*>::const_iterator iter = set.begin();
          iter != set.end(); ++iter ) {
      sched_set.insert
        ( new ScheduleMultiDim<T_DATA> ( *( (*iter)->content() ) ) );
    }

    // perform the combination
    float res = nbOperations ( sched_set, schedule );
    
    // deallocate the wrappers we just constructed
    for ( typename Set<const ScheduleMultiDim<T_DATA>*>::const_iterator
            iter = sched_set.begin(); iter != sched_set.end(); ++iter ) {
      delete *iter;
    }

    return res;
  }

  
  /// returns the memory consumption used during the combination
  template <typename T_DATA>
  std::pair<long,long> ScheduleCombination<T_DATA>::memoryUsage
  ( const Set<const MultiDimImplementation<T_DATA>*>& set,
    const Schedule<T_DATA>& schedule ) {
    // first wrap the multidimimplementations into ScheduleMultiDims
    Set<const ScheduleMultiDim<T_DATA>*> sched_set;
    for ( typename Set<const MultiDimImplementation<T_DATA>*>::const_iterator
            iter = set.begin(); iter != set.end(); ++iter ) {
      sched_set.insert ( new ScheduleMultiDim<T_DATA> ( **iter ) );
    }
    
    // compute the memory consumption of the combination
    std::pair<long,long> res = memoryUsage ( sched_set, schedule );
    
    // deallocate the wrappers we just constructed
    for ( typename Set<const ScheduleMultiDim<T_DATA>*>::const_iterator
            iter = sched_set.begin(); iter != sched_set.end(); ++iter ) {
      delete *iter;
    }
    
    return res;
  }

  
  /** @brief returns a rough estimate of the number of operations that will be
   * performed to compute the combination */
  template <typename T_DATA>
  template <template<typename> class TABLE>
  std::pair<long,long> ScheduleCombination<T_DATA>::memoryUsage
  ( const Set<const TABLE<T_DATA>*>& set,
    const Schedule<T_DATA>& schedule ) {
    // first wrap the TABLES into ScheduleMultiDims
    Set<const ScheduleMultiDim<T_DATA>*> sched_set;
    for ( typename Set<const TABLE<T_DATA>*>::const_iterator iter = set.begin();
          iter != set.end(); ++iter ) {
      sched_set.insert
        ( new ScheduleMultiDim<T_DATA> ( *( (*iter)->content() ) ) );
    }

    // compute the memory consumption of the combination
    std::pair<long,long> res = memoryUsage ( sched_set, schedule );
    
    // deallocate the wrappers we just constructed
    for ( typename Set<const ScheduleMultiDim<T_DATA>*>::const_iterator
            iter = sched_set.begin(); iter != sched_set.end(); ++iter ) {
      delete *iter;
    }

    return res;
  }  
  
  
} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
