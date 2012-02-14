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
 * @brief a MultiDim Delete operator class used for scheduling inferences
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS


#include <limits>
#include <agrum/config.h>

// to ease ide parser
#include <agrum/graphicalModels/inference/scheduleDeleteMultiDim.h>

namespace gum {


  /// default constructor
  template <typename T_DATA>
  ScheduleDeleteMultiDim<T_DATA>::ScheduleDeleteMultiDim
  ( const ScheduleMultiDim<T_DATA>& table ) :
      ScheduleOperation<T_DATA> ( ScheduleOperation<T_DATA>::GUM_DELETE_MULTIDIM ),
      __table( table ),
      __args( 0 ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( ScheduleDeleteMultiDim );
  }


  /// copy constructor
  template <typename T_DATA>
  ScheduleDeleteMultiDim<T_DATA>::ScheduleDeleteMultiDim
  ( const ScheduleDeleteMultiDim<T_DATA>& from ) :
      ScheduleOperation<T_DATA> ( from ),
      __table( from.__table ),
      __args( 0 ) {
    // for debugging purposes
    GUM_CONS_CPY( ScheduleDeleteMultiDim );
  }


  /// virtual copy constructor: creates a clone of the operation
  template <typename T_DATA>
  ScheduleDeleteMultiDim<T_DATA>*
  ScheduleDeleteMultiDim<T_DATA>::newFactory() const {
    return new ScheduleDeleteMultiDim<T_DATA> ( *this );
  }


  /// destructor
  template <typename T_DATA>
  ScheduleDeleteMultiDim<T_DATA>::~ScheduleDeleteMultiDim() {
    // for debugging purposes
    GUM_DESTRUCTOR( ScheduleDeleteMultiDim );
    if ( __args ) delete __args;
  }


  /// copy operator
  template <typename T_DATA>
  ScheduleDeleteMultiDim<T_DATA>&
  ScheduleDeleteMultiDim<T_DATA>::operator=
  ( const ScheduleDeleteMultiDim<T_DATA>& from ) {
    // avoid self assignment
    if ( &from != this ) {
      ScheduleOperation<T_DATA>::operator= ( from );
      __table = from.__table;
      if ( __args ) {
        __args->clear();
        __args->insert( &__table );
      }
    }
    return *this;
  }


  /// operator ==
  template <typename T_DATA>
  bool ScheduleDeleteMultiDim<T_DATA>::operator==
  ( const ScheduleOperation<T_DATA>& op ) const {
    if ( this->type() != op.type() ) return false;
    const ScheduleDeleteMultiDim<T_DATA>& real_op =
      static_cast<const ScheduleDeleteMultiDim<T_DATA>&>( op );
    return __table == real_op.__table;
  }


  /// operator !=
  template <typename T_DATA>
  bool ScheduleDeleteMultiDim<T_DATA>::operator!=
  ( const ScheduleOperation<T_DATA>& op ) const {
    if ( this->type() != op.type() ) return true;
    const ScheduleDeleteMultiDim<T_DATA>& real_op =
      static_cast<const ScheduleDeleteMultiDim<T_DATA>&>( op );
    return __table != real_op.__table;
  }


  /// executes the operation
  template <typename T_DATA>
  void ScheduleDeleteMultiDim<T_DATA>::execute() {
    const MultiDimImplementation<T_DATA>& multidim = __table.multiDim();
    ScheduleMultiDim<T_DATA>::__multidim2id().erase( &multidim );
    ScheduleMultiDim<T_DATA>::__id2multidim().erase( __table.id() );
    delete &multidim;
  }


  /** @brief returns an estimation of the number of elementary operations
   * needed to perform the ScheduleOperation */
  template <typename T_DATA>
  INLINE float ScheduleDeleteMultiDim<T_DATA>::nbOperations() const {
    return 1.0f;
  }


  /// returns the memory consumption used during the operation
  template <typename T_DATA>
  INLINE std::pair<long,long>
  ScheduleDeleteMultiDim<T_DATA>::memoryUsage() const {
    long size_table = __table.domainSize();
    if ( size_table < 0 ) {
      GUM_ERROR( OutOfBounds, "memory usage out of long int range" );
    }
    return std::pair<long,long> ( -size_table,-size_table );
  }


  /// returns the multidims to be deleted
  template <typename T_DATA>
  INLINE const Sequence<const ScheduleMultiDim<T_DATA>*>&
  ScheduleDeleteMultiDim<T_DATA>::multiDimArgs() const {
    if ( ! __args ) {
      __args = new Sequence<const ScheduleMultiDim<T_DATA>*>;
      __args->insert( &__table );
    }
    return *__args;
  }


  /// returns the set of multidims that should be the result of the operation
  template <typename T_DATA>
  INLINE const Sequence<const ScheduleMultiDim<T_DATA>*>&
  ScheduleDeleteMultiDim<T_DATA>::multiDimResults() const {
    static Sequence<const ScheduleMultiDim<T_DATA>*> empty_seq;
#ifndef NDEBUG
    // for debugging purposes, we should inform the aGrUM's debugger that
    // the static sequence used here will be removed at the end of the
    // program's execution.
    static bool first_time = true;
    if ( first_time ) {
      first_time = false;
      debug::__inc_deletion( "Sequence", __FILE__, __LINE__, "destructor of",
                             ( void* ) &empty_seq );
      debug::__inc_deletion( "HashTable", __FILE__, __LINE__, "destructor of",
                             ( void* ) &empty_seq );
      debug::__inc_deletion( "SequenceIterator", __FILE__, __LINE__,
                             "destructor of", ( void* ) &empty_seq );
      debug::__inc_deletion( "SequenceIterator", __FILE__, __LINE__,
                             "destructor of", ( void* ) &empty_seq );
    }
#endif /* NDEBUG */
    return empty_seq;
  }


  /// displays the content of the operation
  template <typename T_DATA>
  std::string ScheduleDeleteMultiDim<T_DATA>::toString() const {
    return "delete ( " + __table.toString() + " )";
  }


} /* namespace */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
