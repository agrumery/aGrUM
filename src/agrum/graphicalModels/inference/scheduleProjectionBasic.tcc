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
#include <agrum/graphicalModels/inference/scheduleProject.h>


namespace gum {


  /// default constructor
  template<typename T_DATA>
  ScheduleProjectionBasic<T_DATA>::ScheduleProjectionBasic
  ( MultiDimImplementation<T_DATA>* (*proj)
    ( const MultiDimImplementation<T_DATA>&,
      const Set<const DiscreteVariable*>& ) ) :
    ScheduleProjection<T_DATA> (),
    _proj ( proj ) {
    /// for debugging purposes
    GUM_CONSTRUCTOR ( ScheduleProjectionBasic );
  }
  

  /// copy constructor
  template<typename T_DATA>
  ScheduleProjectionBasic<T_DATA>::ScheduleProjectionBasic
  ( const ScheduleProjectionBasic<T_DATA>& from ) :
    ScheduleProjection<T_DATA> ( from ),
    _proj ( from._proj ) {
    /// for debugging purposes
    GUM_CONS_CPY ( ScheduleProjectionBasic );
  }

  
  /// destructor
  template<typename T_DATA>
  ScheduleProjectionBasic<T_DATA>::~ScheduleProjectionBasic () {
    /// for debugging purposes
    GUM_DESTRUCTOR ( ScheduleProjectionBasic );
  }
  

  /// virtual constructor
  template<typename T_DATA>
  ScheduleProjectionBasic<T_DATA>*
  ScheduleProjectionBasic<T_DATA>::newFactory () const {
    return new ScheduleProjectionBasic<T_DATA> ( *this );
  }


  /// creates and returns the projection of the table over a subset of its vars
  template<typename T_DATA>
  ScheduleMultiDim<T_DATA>
  ScheduleProjectionBasic<T_DATA>::project
  ( const ScheduleMultiDim<T_DATA>& table,
    const Set<const DiscreteVariable*>& del_vars,
    Schedule<T_DATA>& schedule ) {
    // create the projection that will be performed later on
    ScheduleProject<T_DATA> proj ( table, del_vars, _proj );
    schedule.insert ( proj );

    return proj.result ();
  }

  
  /// creates and returns the projection of the table over a subset of its vars
  template<typename T_DATA>
  INLINE ScheduleMultiDim<T_DATA>
  ScheduleProjectionBasic<T_DATA>::project
  ( const MultiDimImplementation<T_DATA>& table,
    const Set<const DiscreteVariable*>& del_vars,
    Schedule<T_DATA>& schedule ) {
    return ScheduleProjection<T_DATA>::project ( table, del_vars, schedule );
  }

  
  /// creates and returns the projection of the table over a subset of its vars
  template<typename T_DATA>
  template <template<typename> class TABLE>
  INLINE ScheduleMultiDim<T_DATA>
  ScheduleProjectionBasic<T_DATA>::project
  ( const TABLE<T_DATA>& table,
    const Set<const DiscreteVariable*>& del_vars,
    Schedule<T_DATA>& schedule ) {
    return ScheduleProjection<T_DATA>::project ( table, del_vars, schedule );
  }
  

  /// changes the function used for projecting tables
  template<typename T_DATA>
  void ScheduleProjectionBasic<T_DATA>::setProjector
  ( MultiDimImplementation<T_DATA>* (*proj)
    ( const MultiDimImplementation<T_DATA>&,
      const Set<const DiscreteVariable*>& ) ) {
    _proj = proj;
  }
    

  /** @brief returns a rough estimate of the number of operations that will be
   * performed to compute the projection */
  template<typename T_DATA>
  float ScheduleProjectionBasic<T_DATA>::nbOperations
  ( const ScheduleMultiDim<T_DATA>& table,
    const Set<const DiscreteVariable*>& del_vars,
    const Schedule<T_DATA>& schedule ) {
    const Sequence<const DiscreteVariable*>& vars = table.variablesSequence ();
    float res = 1;
    for ( typename Sequence<const DiscreteVariable*>::const_iterator
            iter = vars.begin (); iter != vars.end(); ++iter ) {
      res *= (*iter)->domainSize ();
    }
    return res;
  }
  

  /** @brief returns a rough estimate of the number of operations that will be
   * performed to compute the projection */
  template<typename T_DATA>
  INLINE float ScheduleProjectionBasic<T_DATA>::nbOperations
  ( const  MultiDimImplementation<T_DATA>& table,
    const Set<const DiscreteVariable*>& del_vars,
    const Schedule<T_DATA>& schedule ) {
    return ScheduleProjection<T_DATA>::nbOperations ( table, del_vars, schedule );
  }


  /** @brief returns a rough estimate of the number of operations that will be
   * performed to compute the projection */
  template<typename T_DATA>
  template <template<typename> class TABLE>
  INLINE float ScheduleProjectionBasic<T_DATA>::nbOperations
  ( const TABLE<T_DATA>& table,
    const Set<const DiscreteVariable*>& del_vars,
    const Schedule<T_DATA>& schedule ) {
    return ScheduleProjection<T_DATA>::nbOperations ( table, del_vars, schedule );
  }


} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */

