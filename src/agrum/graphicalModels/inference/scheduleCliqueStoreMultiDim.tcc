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
 * @brief an operator used by scheduled inferences to store tables into cliques
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS


#include <sstream>
#include <agrum/core/debug.h>


namespace gum {

  
  /// default constructor
  template <typename T_DATA>
  ScheduleCliqueStoreMultiDim<T_DATA>::ScheduleCliqueStoreMultiDim
  ( const ScheduleMultiDim<T_DATA>& table,
    typename Property<Set<const MultiDimImplementation<T_DATA>*> >::onNodes&
    clique_tables,
    NodeId clique ) :
    ScheduleOperation<T_DATA>
    ( ScheduleOperation<T_DATA>::GUM_CLIQUE_STORE_MULTIDIM ),
    __table ( table ),
    __tableSet ( &clique_tables ),
    __clique ( clique ),
    __args ( 0 ) {
    // for debugging purposes
    GUM_CONSTRUCTOR ( ScheduleCliqueStoreMultiDim );
  }


  /// copy constructor
  template <typename T_DATA>
  ScheduleCliqueStoreMultiDim<T_DATA>::ScheduleCliqueStoreMultiDim
  ( const ScheduleCliqueStoreMultiDim<T_DATA>& from ) :
    ScheduleOperation<T_DATA> ( from ),
    __table ( from.__table ),
    __tableSet ( from.__tableSet ),
    __clique ( from.__clique ),
    __args ( 0 ) {
    // for debugging purposes
    GUM_CONS_CPY ( ScheduleCliqueStoreMultiDim );
  }

  
  /// virtual copy constructor: creates a clone of the operation
  template <typename T_DATA>
  ScheduleCliqueStoreMultiDim<T_DATA>*
  ScheduleCliqueStoreMultiDim<T_DATA>::newFactory () const {
    return new ScheduleCliqueStoreMultiDim<T_DATA> ( *this );
  }


  /// destructor
  template <typename T_DATA>
  ScheduleCliqueStoreMultiDim<T_DATA>::~ScheduleCliqueStoreMultiDim () {
    // for debugging purposes
    GUM_DESTRUCTOR ( ScheduleCliqueStoreMultiDim );
    if ( __args ) delete __args;
  }


  /// copy operator
  template <typename T_DATA>
  ScheduleCliqueStoreMultiDim<T_DATA>&
  ScheduleCliqueStoreMultiDim<T_DATA>::operator=
  ( const ScheduleCliqueStoreMultiDim<T_DATA>& from ) {
    // avoid self assignment
    if ( &from != this ) {
      ScheduleOperation<T_DATA>::operator= ( from );
      __table = from.__table;
      __tableSet = from.__tableSet;
      __clique = from.__clique;
      if ( __args ) {
        __args->clear ();
        __args->insert ( &__table );
      }
    }
    return *this;
  }


  /// operator ==
  template <typename T_DATA>
  bool ScheduleCliqueStoreMultiDim<T_DATA>::operator==
  ( const ScheduleOperation<T_DATA>& op ) const {
    if ( this->type () != op.type () ) return false;
    const ScheduleCliqueStoreMultiDim<T_DATA>& real_op =
      static_cast<const ScheduleCliqueStoreMultiDim<T_DATA>&> ( op );
    return ( ( __table == real_op.__table ) &&
             ( __tableSet == real_op.__tableSet ) &&
             ( __clique == real_op.__clique ) );
  }


  /// operator !=
  template <typename T_DATA>
  bool ScheduleCliqueStoreMultiDim<T_DATA>::operator!=
  ( const ScheduleOperation<T_DATA>& op ) const {
    if ( this->type () != op.type () ) return true;
    const ScheduleCliqueStoreMultiDim<T_DATA>& real_op =
      static_cast<const ScheduleCliqueStoreMultiDim<T_DATA>&> ( op );
    return ( ( __table != real_op.__table ) ||
             ( __tableSet != real_op.__tableSet ) ||
             ( __clique != real_op.__clique ) );
  }


  /// executes the operation
  template <typename T_DATA>
  void ScheduleCliqueStoreMultiDim<T_DATA>::execute () {
    const MultiDimImplementation<T_DATA>& multidim = __table.multiDim ();
    if ( ! __tableSet->exists ( __clique ) ) {
      __tableSet->insert ( __clique,
                           Set<const MultiDimImplementation<T_DATA>*>() );
    }
    __tableSet->operator[](__clique).insert ( &multidim );
  }
  

  /// returns the multidim to be stored
  template <typename T_DATA>
  INLINE const Sequence<const ScheduleMultiDim<T_DATA>*>&
  ScheduleCliqueStoreMultiDim<T_DATA>::multiDimArgs () const {
    if ( ! __args ) {
      __args = new Sequence<const ScheduleMultiDim<T_DATA>*>;
      __args->insert ( &__table );
    }
    return *__args;
  }


  /// returns the set of multidims that should be the result of the operation
  template <typename T_DATA>
  INLINE const Sequence<const ScheduleMultiDim<T_DATA>*>&
  ScheduleCliqueStoreMultiDim<T_DATA>::multiDimResults () const {
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
  std::string ScheduleCliqueStoreMultiDim<T_DATA>::toString () const {
    std::stringstream s;
    s << "store ( " << __table.toString() << ", clique " << __clique << " )";
    return s.str();
  }
  

} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
