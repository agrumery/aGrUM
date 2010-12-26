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
 * @brief A generic class to project efficiently a ScheduleMultiDim over a subset
 * of its variables
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS


#include <agrum/core/debug.h>


namespace gum {


  /// default constructor
  template<typename T_DATA>
  ScheduleProjection<T_DATA>::ScheduleProjection () {
    /// for debugging purposes
    GUM_CONSTRUCTOR ( ScheduleProjection );
  }
  

  /// copy constructor
  template<typename T_DATA>
  ScheduleProjection<T_DATA>::ScheduleProjection
  ( const ScheduleProjection<T_DATA>& from ) {
    /// for debugging purposes
    GUM_CONS_CPY ( ScheduleProjection );
  }

  
  /// destructor
  template<typename T_DATA>
  ScheduleProjection<T_DATA>::~ScheduleProjection () {
    /// for debugging purposes
    GUM_DESTRUCTOR ( ScheduleProjection );
  }
  

  /// virtual constructor
  template<typename T_DATA>
  ScheduleProjection<T_DATA>* ScheduleProjection<T_DATA>::newFactory () const {
    return new ScheduleProjection<T_DATA> ( *this );
  }

  
  /// creates and returns the projection of the table over a subset of its vars
  template<typename T_DATA>
  INLINE ScheduleMultiDim<T_DATA>
  ScheduleProjection<T_DATA>::project
  ( const MultiDimImplementation<T_DATA>& table,
    const Set<const DiscreteVariable*>& del_vars,
    Schedule<T_DATA>& schedule ) {
    ScheduleMultiDim<T_DATA> sched_table ( table );
    return project ( sched_table, del_vars, schedule );
  }

  
  /// creates and returns the projection of the table over a subset of its vars
  template<typename T_DATA>
  template <template<typename> class TABLE>
  INLINE ScheduleMultiDim<T_DATA>
  ScheduleProjection<T_DATA>::project
  ( const TABLE<T_DATA>& table,
    const Set<const DiscreteVariable*>& del_vars,
    Schedule<T_DATA>& schedule ) {
    ScheduleMultiDim<T_DATA> sched_table ( *( table.getContent () ) );
    return project ( sched_table, del_vars, schedule );
  }
  
  
  /** @brief returns a rough estimate of the number of operations that will be
   * performed to compute the projection */
  template<typename T_DATA>
  INLINE float ScheduleProjection<T_DATA>::nbOperations
  ( const MultiDimImplementation<T_DATA>& table,
    const Set<const DiscreteVariable*>& del_vars,
    const Schedule<T_DATA>& schedule ) {
    ScheduleMultiDim<T_DATA> sched_table ( table );
    return nbOperations ( sched_table, del_vars, schedule );
  }


  /** @brief returns a rough estimate of the number of operations that will be
   * performed to compute the projection */
  template<typename T_DATA>
  template <template<typename> class TABLE>
  INLINE float ScheduleProjection<T_DATA>::nbOperations
  ( const TABLE<T_DATA>& table,
    const Set<const DiscreteVariable*>& del_vars,
    const Schedule<T_DATA>& schedule ) {
    ScheduleMultiDim<T_DATA> sched_table ( *( table.getContent () ) );
    return nbOperations ( sched_table, del_vars, schedule );
  }

  
  /// returns the memory consumption used during the projection
  template<typename T_DATA>
  INLINE std::pair<long,long>
  ScheduleProjection<T_DATA>::memoryUsage
  ( const MultiDimImplementation<T_DATA>& table,
    const Set<const DiscreteVariable*>& del_vars,
    const Schedule<T_DATA>& schedule ) {
    ScheduleMultiDim<T_DATA> sched_table ( table );
    return memoryUsage ( sched_table, del_vars, schedule );
  }

  
  /// returns the memory consumption used during the projection
  template<typename T_DATA>
  template <template<typename> class TABLE>
  INLINE std::pair<long,long>
  ScheduleProjection<T_DATA>::memoryUsage
  ( const TABLE<T_DATA>& table,
    const Set<const DiscreteVariable*>& del_vars,
    const Schedule<T_DATA>& schedule ) {
    ScheduleMultiDim<T_DATA> sched_table ( *( table.getContent () ) );
    return memoryUsage ( sched_table, del_vars, schedule );
  }


} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */

