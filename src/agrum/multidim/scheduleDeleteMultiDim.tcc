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


#include <agrum/core/debug.h>
#include <agrum/core/hashTable.h>
#include <agrum/core/types.h>


namespace gum {


  /// default constructor
  template <typename T_DATA>
  ScheduleDeleteMultiDim<T_DATA>::ScheduleDeleteMultiDim
  ( const ScheduleMultiDim<T_DATA>* table ) :
    ScheduleOperation<T_DATA> ( ScheduleOperation<T_DATA>::GUM_DELETE_MULTIDIM ),
    __table ( table ) {
    // for debugging purposes
    GUM_CONSTRUCTOR ( ScheduleDeleteMultiDim );
  }
    
      
  /// copy constructor
  template <typename T_DATA>
  ScheduleDeleteMultiDim<T_DATA>::ScheduleDeleteMultiDim
  ( const ScheduleDeleteMultiDim<T_DATA>& from ) :
    ScheduleOperation<T_DATA> ( ScheduleOperation<T_DATA>::GUM_DELETE_MULTIDIM ),
    __table ( from.__table ) {
    // for debugging purposes
    GUM_CONS_CPY ( ScheduleDeleteMultiDim );
  }

  
  /// virtual copy constructor: creates a clone of the operation
  template <typename T_DATA>
  ScheduleDeleteMultiDim<T_DATA>*
  ScheduleDeleteMultiDim<T_DATA>::newFactory () const {
    return new ScheduleDeleteMultiDim<T_DATA> ( *this );
  }


  /// destructor
  template <typename T_DATA>
  ScheduleDeleteMultiDim<T_DATA>::~ScheduleDeleteMultiDim () {
    // for debugging purposes
    GUM_DESTRUCTOR ( ScheduleDeleteMultiDim );
  }

  
  /// copy operator
  template <typename T_DATA>
  ScheduleDeleteMultiDim<T_DATA>&
  ScheduleDeleteMultiDim<T_DATA>::operator=
  ( const ScheduleDeleteMultiDim<T_DATA>& from ) {
    __table = from.__table;
    return *this;
  }

  
  /// operator ==
  template <typename T_DATA>
  bool ScheduleDeleteMultiDim<T_DATA>::operator==
  ( const ScheduleOperation<T_DATA>& op ) const {
    if ( this->type () != op.type () ) return false;
    const ScheduleDeleteMultiDim<T_DATA>& real_op =
      static_cast<const ScheduleDeleteMultiDim<T_DATA>&> ( op );
    return *__table == * (real_op.__table );
  }

  
  /// operator !=
  template <typename T_DATA>
  bool ScheduleDeleteMultiDim<T_DATA>::operator!=
  ( const ScheduleOperation<T_DATA>& op ) const {
    if ( this->type () != op.type () ) return true;
    const ScheduleDeleteMultiDim<T_DATA>& real_op =
      static_cast<const ScheduleDeleteMultiDim<T_DATA>&> ( op );
    return *__table != * (real_op.__table );
  }
  

  /// executes the operation
  template <typename T_DATA>
  void ScheduleDeleteMultiDim<T_DATA>::execute () {
    const MultiDimImplementation<T_DATA>& multidim = __table->multiDim ();
    HashTable<Id,const MultiDimImplementation<T_DATA>*>& tables =
      ScheduleMultiDim<T_DATA>::__id2multidims ();
    tables.erase ( __table->id() );
    delete &multidim;
  }

  
  /// returns the multidims to be deleted
  template <typename T_DATA>
  Sequence<const ScheduleMultiDim<T_DATA>*>
  ScheduleDeleteMultiDim<T_DATA>::multiDimArgs () const {
    Sequence<const ScheduleMultiDim<T_DATA>*> seq;
    seq.insert ( __table );
    return seq;
  }

  
  /// displays the content of the operation
  template <typename T_DATA>
  std::string ScheduleDeleteMultiDim<T_DATA>::toString () const {
    return "delete ( " + __table->toString() + " )";
  }

  
} /* namespace */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
