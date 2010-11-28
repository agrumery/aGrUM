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
 * @brief a Projection operator class used for scheduling inferences
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS


#include <agrum/core/debug.h>


namespace gum {


  /// default constructor
  template <typename T_DATA>
  ScheduleProject<T_DATA>::ScheduleProject
  ( const ScheduleMultiDim<T_DATA>* table,
    const Set<const DiscreteVariable *>& del_vars,
    MultiDimImplementation<T_DATA>*
    (*project) ( const MultiDimImplementation<T_DATA>&,
                 const Set<const DiscreteVariable *>& ) ) :
    ScheduleOperation<T_DATA> ( ScheduleOperation<T_DATA>::GUM_PROJECT_MULTIDIM ),
    __table ( table ),
    __del_vars ( del_vars ),
    __project ( project ) {
    // for debugging purposes
    GUM_CONSTRUCTOR ( ScheduleProject );

    // compute the variables that shall belong to the result of the projection
    Sequence<const DiscreteVariable*> vars = __table->variables ();
    for ( typename Set<const DiscreteVariable*>::const_iterator
            iter = del_vars.begin(); iter != del_vars.end(); ++iter ) {
      vars.erase ( *iter );
    }

    // create the scheduleMultiDim that should result from the combination of
    // table1 and table2
    __result = new ScheduleMultiDim<T_DATA> ( vars );
  }


  /// copy constructor
  template <typename T_DATA>
  ScheduleProject<T_DATA>::ScheduleProject ( const ScheduleProject<T_DATA>& f ) :
    ScheduleOperation<T_DATA> ( ScheduleOperation<T_DATA>::GUM_PROJECT_MULTIDIM ),
    __table ( f.__table ),
    __del_vars ( f.__del_vars ),
    __result ( new ScheduleMultiDim<T_DATA> ( *(f.__result ) ) ),
    __project ( f.__project ) {
    // for debugging purposes
    GUM_CONS_CPY ( ScheduleProject );
  }
  

  /// destructor
  template <typename T_DATA>
  ScheduleProject<T_DATA>::~ScheduleProject () {
    // for debugging purposes
    GUM_DESTRUCTOR ( ScheduleProject );
    delete __result;
  }
    

  /// copy operator
  template <typename T_DATA>
  ScheduleProject<T_DATA>&
  ScheduleProject<T_DATA>::operator= ( const ScheduleProject<T_DATA>& from ) {
    // avoid self assignment
    if ( this != &from ) {
      __table = from.__table;
      __del_vars = from.__del_vars;
      *__result = *( from.__result );
      __project = from.__project;
    }
    return *this;
  }

  
  /// operator ==
  template <typename T_DATA>
  bool ScheduleProject<T_DATA>::operator==
  ( const ScheduleOperation<T_DATA>& op ) const {
    if ( this->type () != op.type () ) return false;
    const ScheduleProject<T_DATA>& real_op =
      static_cast<const ScheduleProject<T_DATA>&> ( op );
    return ( ( *__table == *(real_op.__table) ) &&
             ( __del_vars == real_op.__del_vars) &&
             ( __project == real_op.__project ) );
  }

  
  /// operator !=
  template <typename T_DATA>
  bool ScheduleProject<T_DATA>::operator!=
  ( const ScheduleOperation<T_DATA>& op ) const {
    if ( this->type () != op.type () ) return true;
    const ScheduleProject<T_DATA>& real_op =
      static_cast<const ScheduleProject<T_DATA>&> ( op );
    return ( ( *__table != *(real_op.__table) ) ||
             ( __del_vars != real_op.__del_vars) ||
             ( __project != real_op.__project ) );
  }

 
  /// executes the operation
  template <typename T_DATA>
  void ScheduleProject<T_DATA>::execute () {
    if ( __result->isAbstract () ) {
      const MultiDimImplementation<T_DATA>& t = __table->multiDim ();
      MultiDimImplementation<T_DATA>* res = __project ( t, __del_vars );
      __result->setMultiDim ( *res );
    }
  }
  

  /// returns the scheduleMultidim resulting from the execution of the operation
  template <typename T_DATA>
  INLINE const ScheduleMultiDim<T_DATA>&
  ScheduleProject<T_DATA>::result () const {
    return *__result;
  }

  
  /// returns the set of multidims passed in argument to the operation
  template <typename T_DATA>
  Sequence<const ScheduleMultiDim<T_DATA>*>
  ScheduleProject<T_DATA>::multiDimArgs () const {
    Sequence<const ScheduleMultiDim<T_DATA>*> set;
    set.insert ( __table );
    return set;
  }

  
  /// displays the content of the operation
  template <typename T_DATA>
  std::string ScheduleProject<T_DATA>::toString () const {
    return __result->toString() + " = project ( " +
      __table->toString() + " , " + __del_vars.toString() + " )";
  }


} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
