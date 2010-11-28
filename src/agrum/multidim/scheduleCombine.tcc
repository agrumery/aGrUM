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
 * @brief a Combination operator class used for scheduling inferences
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS


#include <agrum/core/debug.h>


namespace gum {


  /// default constructor
  template <typename T_DATA>
  ScheduleCombine<T_DATA>::ScheduleCombine
  ( const ScheduleMultiDim<T_DATA>* table1,
    const ScheduleMultiDim<T_DATA>* table2,
    MultiDimImplementation<T_DATA>*
    (*combine) ( const MultiDimImplementation<T_DATA>&,
                 const MultiDimImplementation<T_DATA>& ) ) :
    ScheduleOperation<T_DATA> ( ScheduleOperation<T_DATA>::GUM_COMBINE_MULTIDIM ),
    __table1 ( table1 ),
    __table2 ( table2 ),
    __combine ( combine ) {
    // for debugging purposes
    GUM_CONSTRUCTOR ( ScheduleCombine );

    // compute the variables of the resulting table
    Sequence<const DiscreteVariable*> vars = __table1->variables ();
    const Sequence<const DiscreteVariable*>& vars2 = __table2->variables ();
    for ( typename Sequence<const DiscreteVariable*>::const_iterator
            iter = vars2.begin(); iter != vars2.end(); ++iter ) {
      if ( ! vars.exists ( *iter ) ) {
        vars.insert ( *iter );
      }
    }

    // create the scheduleMultiDim that should result from the combination of
    // table1 and table2
    __result = new ScheduleMultiDim<T_DATA> ( vars );
  }

  
  /// copy constructor
  template <typename T_DATA>
  ScheduleCombine<T_DATA>::ScheduleCombine
  ( const ScheduleCombine<T_DATA>& from ) :
    ScheduleOperation<T_DATA> ( ScheduleOperation<T_DATA>::GUM_COMBINE_MULTIDIM ),
    __table1 ( from.__table1 ),
    __table2 ( from.__table2 ),
    __result ( new ScheduleMultiDim<T_DATA> ( *(from.__result) ) ),
    __combine ( from.__combine ) {
    // for debugging purposes
    GUM_CONS_CPY ( ScheduleCombine );
  }


  /// destructor
  template <typename T_DATA>
  ScheduleCombine<T_DATA>::~ScheduleCombine () {
    // for debugging purposes
    GUM_DESTRUCTOR ( ScheduleCombine );
    delete __result;
  }

  
  /// copy operator
  template <typename T_DATA>
  ScheduleCombine<T_DATA>&
  ScheduleCombine<T_DATA>::operator= ( const ScheduleCombine<T_DATA>& from ) {
    // avoid self assignment
    if ( this != &from ) {
      __table1 = from.__table1;
      __table2 = from.__table2;
      *__result = *( from.__result );
      __combine = from.__combine;
    }
    return *this;
  }

 
  /// operator ==
  template <typename T_DATA>
  INLINE bool
  ScheduleCombine<T_DATA>::operator==
  ( const ScheduleOperation<T_DATA>& op ) const {
    if ( this->type () != op.type () ) return false;
    const ScheduleCombine<T_DATA>& real_op =
      static_cast<const ScheduleCombine<T_DATA>&> ( op );
    return ( ( ( ( *__table1 == *(real_op.__table1) ) &&
                 ( *__table2 == *(real_op.__table2) ) ) ||
               ( ( *__table1 == *(real_op.__table2) ) &&
                 ( *__table2 == *(real_op.__table1) ) ) ) &&
             ( __combine == real_op.__combine ) );
  }


  /// operator !=
  template <typename T_DATA>
  INLINE bool
  ScheduleCombine<T_DATA>::operator!=
  ( const ScheduleOperation<T_DATA>& op ) const {
    if ( this->type () != op.type () ) return true;
    const ScheduleCombine<T_DATA>& real_op =
      static_cast<const ScheduleCombine<T_DATA>&> ( op );
    return ( ( *__table1 != *(real_op.__table1) ) ||
             ( *__table2 != *(real_op.__table2) ) ||
             ( __combine != real_op.__combine ) );
  }

  
  /// executes the operation
  template <typename T_DATA>
  void ScheduleCombine<T_DATA>::execute () {
    if (  __result->isAbstract () ) {
      // first, get the multidims to combine
      const MultiDimImplementation<T_DATA>& t1 = __table1->multiDim ();
      const MultiDimImplementation<T_DATA>& t2 = __table2->multiDim ();

      // perform the combination and store the result
      MultiDimImplementation<T_DATA>* res = __combine ( t1, t2 );
      __result->setMultiDim ( *res );
    }
  }

  
  /// returns the set of multidims passed in argument to the operation
  template <typename T_DATA>
  Sequence<const ScheduleMultiDim<T_DATA>*>
  ScheduleCombine<T_DATA>::multiDimArgs () const {
    Sequence<const ScheduleMultiDim<T_DATA>*> set;
    set.insert ( __table1 );
    set.insert ( __table2 );
    return set;
  }

  
  /// displays the content of the operation
  template <typename T_DATA>
  std::string ScheduleCombine<T_DATA>::toString () const {
    return __result->toString() + " = combine ( " +
      __table1->toString() + " , " + __table2->toString() + " )";
  }


  /// returns the scheduleMultidim resulting from the execution of the operation
  template <typename T_DATA>
  INLINE const ScheduleMultiDim<T_DATA>&
  ScheduleCombine<T_DATA>::result () const {
    return *__result;
  }

  
} /* namespace */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
