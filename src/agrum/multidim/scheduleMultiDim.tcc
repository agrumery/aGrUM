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
 * @brief a MultiDimImplementation Wrapper used for scheduling inferences
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */


#ifndef DOXYGEN_SHOULD_SKIP_THIS


#include <sstream>
#include <string>
#include <agrum/core/debug.h>


namespace gum {
  

  /// returns a new distinct ID for each abtract scheduleMultiDim
  template <typename T_DATA>
  Id ScheduleMultiDim<T_DATA>::__newId () {
    static Id __abstract_id = 0;
    return ++__abstract_id;
  }
 

  /// returns a mapping from id to multidimdecorators
  template <typename T_DATA>
  HashTable<Id,const MultiDimImplementation<T_DATA>*>&
  ScheduleMultiDim<T_DATA>::__id2multidims () {
    static HashTable<Id,const MultiDimImplementation<T_DATA>*> __multidims;
    #ifndef NDEBUG
      // for debugging purposes, we should inform the aGrUM's debugger that
      // the static hashtable used here will be removed at the end of the
      // program's execution.
      static bool first_time = true;
      if ( first_time ) {
        first_time = false;
        debug::__inc_deletion ( "HashTable", __FILE__, __LINE__, "destructor of",
                                (void*) &__multidims );
      }
    #endif /* NDEBUG */
    return __multidims;
  }

  
  /// returns a table indicating how many ScheduleMultiDim have the same id
  template <typename T_DATA>
  HashTable<Id,unsigned int>&
  ScheduleMultiDim<T_DATA>::__id2refs () {
    static HashTable<Id,unsigned int> __ids;
    #ifndef NDEBUG
      // for debugging purposes, we should inform the aGrUM's debugger that
      // the static hashtable used here will be removed at the end of the
      // program's execution.
      static bool first_time = true;
      if ( first_time ) {
        first_time = false;
        debug::__inc_deletion ( "HashTable", __FILE__, __LINE__, "destructor of",
                                (void*) &__ids );
      }
    #endif /* NDEBUG */
    return __ids;
  }


  /// returns a table with the variables of the table corresponding to id
  template <typename T_DATA>
  HashTable<Id,const Sequence<const DiscreteVariable*>*>&
  ScheduleMultiDim<T_DATA>::__id2vars () {
    static HashTable<Id,const Sequence<const DiscreteVariable*>*> __vars;
    #ifndef NDEBUG
      // for debugging purposes, we should inform the aGrUM's debugger that
      // the static hashtable used here will be removed at the end of the
      // program's execution.
      static bool first_time = true;
      if ( first_time ) {
        first_time = false;
        debug::__inc_deletion ( "HashTable", __FILE__, __LINE__, "destructor of",
                                (void*) &__vars );
      }
    #endif /* NDEBUG */
    return __vars;
  }

  
  /// constructs a ScheduleMultiDim containing an already built decorator
  template <typename T_DATA>
  ScheduleMultiDim<T_DATA>::ScheduleMultiDim
  ( const MultiDimImplementation<T_DATA>& multidim ) :
    __id ( __newId () )  {
    // for debugging purposes
    GUM_CONSTRUCTOR ( ScheduleMultiDim );

    // indicate that the id is currently used once
    __id2refs().insert ( __id, 1 );

    // store the multidim into the set of all mappings id->multidim
    __id2multidims().insert ( __id, &multidim );

    // store the variables of the multidim
    const Sequence<const DiscreteVariable*>* vars =
      new Sequence<const DiscreteVariable*> ( multidim.variablesSequence () );
    __id2vars().insert ( __id, vars );
  }
  

  /// constructs a ScheduleMultiDim containing an already built decorator
  template <typename T_DATA>
  ScheduleMultiDim<T_DATA>::ScheduleMultiDim
  ( const MultiDimDecorator<T_DATA>& multidim ) :
    __id ( __newId () )  {
    // for debugging purposes
    GUM_CONSTRUCTOR ( ScheduleMultiDim );

    // indicate that the id is currently used once
    __id2refs().insert ( __id, 1 );

    // store the multidim into the set of all mappings id->multidim
    __id2multidims().insert ( __id, multidim.getContent() );

    // store the variables of the multidim
    const Sequence<const DiscreteVariable*>* vars =
      new Sequence<const DiscreteVariable*> ( multidim.variablesSequence () );
    __id2vars().insert ( __id, vars );
  }
  

  /// construct an (abstract) ScheduleMultiDim for a decorator yet to be built
  template <typename T_DATA>
  ScheduleMultiDim<T_DATA>::ScheduleMultiDim
  ( const Sequence<const DiscreteVariable*>& vars) :
    __id ( __newId () )  {
    // for debugging purposes
    GUM_CONSTRUCTOR ( ScheduleMultiDim );

    // indicate that the id is currently used once
    __id2refs().insert ( __id, 1 );

    // store the variables of the multidim
    const Sequence<const DiscreteVariable*>* new_vars =
      new Sequence<const DiscreteVariable*> ( vars );
    __id2vars().insert ( __id, new_vars );
  }
  

  /// copy constructor
  template <typename T_DATA>
  ScheduleMultiDim<T_DATA>::ScheduleMultiDim
  ( const ScheduleMultiDim<T_DATA>& from ) :
    __id ( from.__id ) {
    // for debugging purposes
    GUM_CONS_CPY ( ScheduleMultiDim );

    // update the number of references of __id
    HashTable<Id,unsigned int>& refs = __id2refs ();
    ++refs[__id];
  }
    
    
  /// destructor
  template <typename T_DATA>
  ScheduleMultiDim<T_DATA>::~ScheduleMultiDim () {
    // for debugging purposes
    GUM_DESTRUCTOR ( ScheduleMultiDim );

    // get the number of scheduleMultiDims that reference the same id
    HashTable<Id,unsigned int>& refs = __id2refs ();
    unsigned int& nb_refs = refs[__id];
    if ( nb_refs == 1 ) {
      // remove the id and the multidimdecorator from the mappings
      refs.erase ( __id );
      delete __id2vars().operator[] ( __id );
      __id2vars().erase ( __id );
      __id2multidims().erase ( __id );
    }
    else
      --nb_refs;
  }


  /// copy operator
  template <typename T_DATA>
  ScheduleMultiDim<T_DATA>&
  ScheduleMultiDim<T_DATA>::operator=
  ( const ScheduleMultiDim<T_DATA>& from ) {
    // avoid self assignment
    if ( id() != from.id() ) {
      // remove the old id
      HashTable<Id,unsigned int>& refs = __id2refs ();
      unsigned int& nb_refs = refs[__id];
      if ( nb_refs == 1 ) {
        // remove the id and the multidimdecorator from the mappings
        refs.erase ( __id );
        delete __id2vars().operator[] ( __id );
        __id2vars().erase ( __id );
        __id2multidims().erase ( __id );
      }
      else
        --nb_refs;
      
      __id = from.__id;
      ++refs[__id];
    }
    return *this;
  }

  
  /// checks whether two ScheduleMultiDim are related to the same table
  template <typename T_DATA>
  INLINE bool
  ScheduleMultiDim<T_DATA>::operator==
  ( const ScheduleMultiDim<T_DATA>& m ) const {
    return ( __id == m.__id );
  }
  
    
  /// checks whether two ScheduleMultiDim are related to different tables
  template <typename T_DATA>
  INLINE bool
  ScheduleMultiDim<T_DATA>::operator!=
  ( const ScheduleMultiDim<T_DATA>& m ) const {
    return ( __id != m.__id );
  }
  

  /// returns the multiDimImplementation actually contained in the ScheduleMultiDim
  template <typename T_DATA>
  INLINE const MultiDimImplementation<T_DATA>&
  ScheduleMultiDim<T_DATA>::multiDim () const {
    return * ( __id2multidims ().operator[] ( __id ) );
  }

  
  /// returns whether the ScheduleMultiDim contains a real multiDimImplementation
  template <typename T_DATA>
  INLINE bool ScheduleMultiDim<T_DATA>::isAbstract () const {
    return  ! __id2multidims ().exists ( __id ); 
  }

  
  /// returns the id of the ScheduleMultiDim
  template <typename T_DATA>
  INLINE Id ScheduleMultiDim<T_DATA>::id () const {
    return __id;
  }

  
  /// returns the set of variables involved in the multidim
  template <typename T_DATA>
  INLINE const Sequence<const DiscreteVariable*>&
  ScheduleMultiDim<T_DATA>::variables () const {
    return * ( __id2vars ().operator[] ( __id ) );
  }

  
  /// sets a new multiDimImplementation inside the wrapper
  template <typename T_DATA>
  INLINE void
  ScheduleMultiDim<T_DATA>::setMultiDim
  ( const MultiDimImplementation<T_DATA>& m ) {
    // store the new multidim
    try { __id2multidims().operator[] ( __id ) = &m; }
    catch ( NotFound& ) { __id2multidims().insert ( __id, &m ); }

    // update the variables of the scheduleMultiDim
    const Sequence<const DiscreteVariable*>& m_vars = m.variablesSequence ();
    Sequence<const DiscreteVariable*>* vars = 
      const_cast<Sequence<const DiscreteVariable*>*>
      ( __id2vars().operator[] ( __id ) );
    *vars = m_vars;
  }
  
    
  /// sets a new multiDimImplementation inside the wrapper
  template <typename T_DATA>
  INLINE void
  ScheduleMultiDim<T_DATA>::setMultiDim
  ( const MultiDimDecorator<T_DATA>& m ) {
    // store the new multidim
    try { __id2multidims().operator[] ( __id ) = m.getContent(); }
    catch ( NotFound& ) { __id2multidims().insert ( __id, m.getContent() ); }

    // update the variables of the scheduleMultiDim
    const Sequence<const DiscreteVariable*>& m_vars = m.variablesSequence ();
    Sequence<const DiscreteVariable*>* vars = 
      const_cast<Sequence<const DiscreteVariable*>*>
      ( __id2vars().operator[] ( __id ) );
    *vars = m_vars;
  }
    

  /// displays the content of the multidim
  template <typename T_DATA>
  std::string ScheduleMultiDim<T_DATA>::toString () const {
    std::stringstream str;
    str << "<";
    
    // get the pointer of the multidim if it exists
    try { str << __id2multidims().operator[] ( __id ); }
    catch ( NotFound& ) { str << __id; }
    str << ">";
    
    return str.str();
  }
 

} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
