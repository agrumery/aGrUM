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
 * @brief an operator used by scheduled inferences to store tables into separators
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS


#include <sstream>
#include <agrum/core/debug.h>


namespace gum {


  /// default constructor
  template <typename T_DATA>
  ScheduleSeparatorStoreMultiDim<T_DATA>::ScheduleSeparatorStoreMultiDim
  ( const ScheduleMultiDim<T_DATA>& table,
    typename Property<Set<const MultiDimImplementation<T_DATA>*> >::onArcs&
    separator_tables,
    Arc separator ) :
    ScheduleOperation<T_DATA>
    ( ScheduleOperation<T_DATA>::GUM_SEPARATOR_STORE_MULTIDIM ),
    __table ( table ),
    __tableSet ( &separator_tables ),
    __separator ( separator ),
    __args ( 0 ) {
    // for debugging purposes
    GUM_CONSTRUCTOR ( ScheduleSeparatorStoreMultiDim );
  }


  /// copy constructor
  template <typename T_DATA>
  ScheduleSeparatorStoreMultiDim<T_DATA>::ScheduleSeparatorStoreMultiDim
  ( const ScheduleSeparatorStoreMultiDim<T_DATA>& from ) :
    ScheduleOperation<T_DATA> ( from ),
    __table ( from.__table ),
    __tableSet ( from.__tableSet ),
    __separator ( from.__separator ),
    __args ( 0 ) {
    // for debugging purposes
    GUM_CONS_CPY ( ScheduleSeparatorStoreMultiDim );
  }

  
  /// virtual copy constructor: creates a clone of the operation
  template <typename T_DATA>
  ScheduleSeparatorStoreMultiDim<T_DATA>*
  ScheduleSeparatorStoreMultiDim<T_DATA>::newFactory () const {
    return new ScheduleSeparatorStoreMultiDim<T_DATA> ( *this );
  }


  /// destructor
  template <typename T_DATA>
  ScheduleSeparatorStoreMultiDim<T_DATA>::~ScheduleSeparatorStoreMultiDim () {
    // for debugging purposes
    GUM_DESTRUCTOR ( ScheduleSeparatorStoreMultiDim );
    if ( __args ) delete __args;
  }


  /// copy operator
  template <typename T_DATA>
  ScheduleSeparatorStoreMultiDim<T_DATA>&
  ScheduleSeparatorStoreMultiDim<T_DATA>::operator=
  ( const ScheduleSeparatorStoreMultiDim<T_DATA>& from ) {
    // avoid self assignment
    if ( &from != this ) {
      ScheduleOperation<T_DATA>::operator= ( from );
      __table = from.__table;
      __tableSet = from.__tableSet;
      __separator = from.__separator;
      if ( __args ) {
        __args->clear ();
        __args->insert ( &__table );
      }
    }
    return *this;
  }


  /// operator ==
  template <typename T_DATA>
  bool ScheduleSeparatorStoreMultiDim<T_DATA>::operator==
  ( const ScheduleOperation<T_DATA>& op ) const {
    if ( this->type () != op.type () ) return false;
    const ScheduleSeparatorStoreMultiDim<T_DATA>& real_op =
      static_cast<const ScheduleSeparatorStoreMultiDim<T_DATA>&> ( op );
    return ( ( __table == real_op.__table ) &&
             ( __tableSet == real_op.__tableSet ) &&
             ( __separator == real_op.__separator ) );
  }


  /// operator !=
  template <typename T_DATA>
  bool ScheduleSeparatorStoreMultiDim<T_DATA>::operator!=
  ( const ScheduleOperation<T_DATA>& op ) const {
    if ( this->type () != op.type () ) return true;
    const ScheduleSeparatorStoreMultiDim<T_DATA>& real_op =
      static_cast<const ScheduleSeparatorStoreMultiDim<T_DATA>&> ( op );
    return ( ( __table != real_op.__table ) ||
             ( __tableSet != real_op.__tableSet ) ||
             ( __separator != real_op.__separator ) );
  }


  /// executes the operation
  template <typename T_DATA>
  void ScheduleSeparatorStoreMultiDim<T_DATA>::execute () {
    const MultiDimImplementation<T_DATA>& multidim = __table.multiDim ();
    if ( ! __tableSet->exists ( __separator ) ) {
      __tableSet->insert ( __separator,
                           Set<const MultiDimImplementation<T_DATA>*>() );
    }
    __tableSet->operator[](__separator).insert ( &multidim );
  }
  

  /** @brief returns an estimation of the number of elementary operations
   * needed to perform the ScheduleOperation */
  template <typename T_DATA>
  INLINE float ScheduleSeparatorStoreMultiDim<T_DATA>::nbOperations () const {
    return 1.0f;
  }


  /// returns the memory consumption used during the operation
  template <typename T_DATA>
  INLINE std::pair<long,long>
  ScheduleSeparatorStoreMultiDim<T_DATA>::memoryUsage () const {
    return std::pair<long,long> (0,0);
  }
 

  /// returns the multidim to be stored
  template <typename T_DATA>
  INLINE const Sequence<const ScheduleMultiDim<T_DATA>*>&
  ScheduleSeparatorStoreMultiDim<T_DATA>::multiDimArgs () const {
    if ( ! __args ) {
      __args = new Sequence<const ScheduleMultiDim<T_DATA>*>;
      __args->insert ( &__table );
    }
    return *__args;
  }


  /// returns the set of multidims that should be the result of the operation
  template <typename T_DATA>
  INLINE const Sequence<const ScheduleMultiDim<T_DATA>*>&
  ScheduleSeparatorStoreMultiDim<T_DATA>::multiDimResults () const {
    static Sequence<const ScheduleMultiDim<T_DATA>*> empty_seq;
    #ifndef NDEBUG
      // for debugging purposes, we should inform the aGrUM's debugger that
      // the static sequence used here will be removed at the end of the
      // program's execution.
      static bool first_time = true;
      if ( first_time ) {
        first_time = false;
        debug::__inc_deletion ( "Sequence", __FILE__, __LINE__, "destructor of",
                                (void*) &empty_seq );
      }
    #endif /* NDEBUG */
    return empty_seq;
  }

  
  /// displays the content of the operation
  template <typename T_DATA>
  std::string ScheduleSeparatorStoreMultiDim<T_DATA>::toString () const {
    std::stringstream s;
    s << "store ( " << __table.toString() << ", separator "
      << __separator << " )";
    return s.str ();
  }

  
} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
