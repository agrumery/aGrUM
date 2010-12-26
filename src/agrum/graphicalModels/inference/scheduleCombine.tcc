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


#include <limits>
#include <agrum/core/debug.h>


namespace gum {


  /// default constructor
  template <typename T_DATA>
  ScheduleCombine<T_DATA>::ScheduleCombine
  ( const ScheduleMultiDim<T_DATA>& table1,
    const ScheduleMultiDim<T_DATA>& table2,
    MultiDimImplementation<T_DATA>*
    (*combine) ( const MultiDimImplementation<T_DATA>&,
                 const MultiDimImplementation<T_DATA>& ) ) :
    ScheduleOperation<T_DATA> ( ScheduleOperation<T_DATA>::GUM_COMBINE_MULTIDIM ),
    __table1 ( table1 ),
    __table2 ( table2 ),
    __args ( 0 ),
    __results ( 0 ),
    __combine ( combine ) {
    // for debugging purposes
    GUM_CONSTRUCTOR ( ScheduleCombine );

    // compute the variables of the resulting table
    Sequence<const DiscreteVariable*> vars = __table1.variablesSequence ();
    const Sequence<const DiscreteVariable*>& vars2 = __table2.variablesSequence ();
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
    ScheduleOperation<T_DATA> ( from ),
    __table1 ( from.__table1 ),
    __table2 ( from.__table2 ),
    __result ( new ScheduleMultiDim<T_DATA> ( *(from.__result) ) ),
    __args ( 0 ),
    __results ( 0 ),
    __combine ( from.__combine ) {
    // for debugging purposes
    GUM_CONS_CPY ( ScheduleCombine );
  }

  
  /// virtual copy constructor: creates a clone of the operation
  template <typename T_DATA>
  ScheduleCombine<T_DATA>* ScheduleCombine<T_DATA>::newFactory () const {
    return new ScheduleCombine<T_DATA> ( *this );
  }


  /// destructor
  template <typename T_DATA>
  ScheduleCombine<T_DATA>::~ScheduleCombine () {
    // for debugging purposes
    GUM_DESTRUCTOR ( ScheduleCombine );
    delete __result;
    if ( __args ) delete __args;
    if ( __results ) delete __results;
  }

  
  /// copy operator
  template <typename T_DATA>
  ScheduleCombine<T_DATA>&
  ScheduleCombine<T_DATA>::operator= ( const ScheduleCombine<T_DATA>& from ) {
    // avoid self assignment
    if ( this != &from ) {
      ScheduleOperation<T_DATA>::operator= ( from );
      __table1 = from.__table1;
      __table2 = from.__table2;
      *__result = *( from.__result );
      __combine = from.__combine;

      // update __args and __results if they were already created
      if ( __args ) {
        __args->clear ();
        __args->insert ( &__table1 );
        __args->insert ( &__table2 );
      }
      if ( __results ) {
        __results->clear ();
        __results->insert ( __result );
      }
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
    return ( ( ( ( __table1 == real_op.__table1 ) &&
                 ( __table2 == real_op.__table2 ) ) ||
               ( ( __table1 == real_op.__table2 ) &&
                 ( __table2 == real_op.__table1 ) ) ) &&
             ( __combine == real_op.__combine ) );
  }


  /// operator !=
  template <typename T_DATA>
  INLINE bool
  ScheduleCombine<T_DATA>::operator!=
  ( const ScheduleOperation<T_DATA>& op ) const {
    return ! operator== ( op );
  }

  
  /// executes the operation
  template <typename T_DATA>
  void ScheduleCombine<T_DATA>::execute () {
    if (  __result->isAbstract () ) {
      // first, get the multidims to combine
      const MultiDimImplementation<T_DATA>& t1 = __table1.multiDim ();
      const MultiDimImplementation<T_DATA>& t2 = __table2.multiDim ();

      // perform the combination and store the result
      MultiDimImplementation<T_DATA>* res = __combine ( t1, t2 );
      __result->setMultiDim ( *res );
    }
  }

  
  /** @brief returns an estimation of the number of elementary operations
   * needed to perform the ScheduleOperation */
  template <typename T_DATA>
  float ScheduleCombine<T_DATA>::nbOperations () const {
    const Sequence<const DiscreteVariable *>& seq1 = __table1.variablesSequence ();
    const Sequence<const DiscreteVariable *>& seq2 = __table2.variablesSequence ();
    if ( seq1.empty() && seq2.empty() ) return 0.0f;
    
    float size = 1;
    for ( Sequence<const DiscreteVariable *>::const_iterator iter =
            seq1.begin(); iter != seq1.end(); ++iter ) {
      size *= (*iter)->domainSize();
    }
    for ( Sequence<const DiscreteVariable *>::const_iterator iter =
            seq2.begin(); iter != seq2.end(); ++iter ) {
      if ( ! seq1.exists ( *iter ) )
        size *= (*iter)->domainSize();
    }
    
    return size;
  }
  
    
  /// returns the memory consumption used during the operation
  template <typename T_DATA>
  std::pair<long,long> ScheduleCombine<T_DATA>::memoryUsage () const {
    const Sequence<const DiscreteVariable *>& seq1 = __table1.variablesSequence ();
    const Sequence<const DiscreteVariable *>& seq2 = __table2.variablesSequence ();
    if ( seq1.empty() && seq2.empty() ) return std::pair<long,long> (0,0);

    long size = 1;
    for ( Sequence<const DiscreteVariable *>::const_iterator iter =
            seq1.begin(); iter != seq1.end(); ++iter ) {
      if ( std::numeric_limits<long>::max() /
           (long) (*iter)->domainSize() < size ) {
        GUM_ERROR ( OutOfBounds, "memory usage out of long int range" );
      }
      size *= (*iter)->domainSize();
    }
    for ( Sequence<const DiscreteVariable *>::const_iterator iter =
            seq2.begin(); iter != seq2.end(); ++iter ) {
      if ( ! seq1.exists ( *iter ) ) {
        if ( std::numeric_limits<long>::max() /
             (long) (*iter)->domainSize() < size ) {
          GUM_ERROR ( OutOfBounds, "memory usage out of long int range" );
        }
        size *= (*iter)->domainSize();
      }
    }
    
    return std::pair<long,long> (size,size);
  }
 
  
  /// returns the set of multidims passed in argument to the operation
  template <typename T_DATA>
  INLINE const Sequence<const ScheduleMultiDim<T_DATA>*>&
  ScheduleCombine<T_DATA>::multiDimArgs () const {
    if ( ! __args ) {
      __args = new Sequence<const ScheduleMultiDim<T_DATA>*>;
      __args->insert ( &__table1 );
      __args->insert ( &__table2 );
    }
    return *__args;
  }


  /// returns the set of multidims that should be the result of the operation
  template <typename T_DATA>
  INLINE const Sequence<const ScheduleMultiDim<T_DATA>*>&
  ScheduleCombine<T_DATA>::multiDimResults () const {
    if ( ! __results ) {
      __results = new Sequence<const ScheduleMultiDim<T_DATA>*>;
      __results->insert ( __result );
    }
    return *__results;
  }
    
 
  /// displays the content of the operation
  template <typename T_DATA>
  std::string ScheduleCombine<T_DATA>::toString () const {
    return __result->toString() + " = combine ( " +
      __table1.toString() + " , " + __table2.toString() + " )";
  }


  /// returns the scheduleMultidim resulting from the execution of the operation
  template <typename T_DATA>
  INLINE const ScheduleMultiDim<T_DATA>&
  ScheduleCombine<T_DATA>::result () const {
    return *__result;
  }

  
} /* namespace */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
