/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr   *
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


namespace gum {


  // ==============================================================================
  /// constructor
  // ==============================================================================
  template<typename T_DATA> INLINE
  MultiDimDecorator<T_DATA>::MultiDimDecorator
  ( MultiDimImplementation<T_DATA> *aContent ) : _content( aContent ) {
    GUM_CONSTRUCTOR( MultiDimDecorator );
  }

  // ==============================================================================
  /// destructor
  // ==============================================================================
  template<typename T_DATA> INLINE
  MultiDimDecorator<T_DATA>::~MultiDimDecorator() {
    if ( _content ) {
      delete _content;
    }

    GUM_DESTRUCTOR( MultiDimDecorator );
  }

  // ==============================================================================
  /// return a data, given a Insantiation - final method
  // ==============================================================================
  template<typename T_DATA> INLINE
  T_DATA& MultiDimDecorator<T_DATA>::_get( const Instantiation& i ) const {
    GUM_ERROR( OperationNotAllowed,"_get in the implementation !" );
  }

  template<typename T_DATA> INLINE
  T_DATA MultiDimDecorator<T_DATA>::get( const Instantiation& i ) const {
    return (( MultiDimContainer<T_DATA> * ) _content )->get( i );
  }

  template<typename T_DATA> INLINE
  void MultiDimDecorator<T_DATA>::set( const Instantiation& i, const T_DATA& value ) const {
    return (( MultiDimContainer<T_DATA> * ) _content )->set( i,value );
  }

  // ==============================================================================
  /// get the size of domains - final method
  // ==============================================================================
  template<typename T_DATA> INLINE
  Size MultiDimDecorator<T_DATA>::domainSize() const  {
    return (( MultiDimContainer<T_DATA> * ) _content )->domainSize();
  }

  // ==============================================================================
  /// add a new var to the sequence of vars - final method
  // ==============================================================================
  template<typename T_DATA> INLINE
  void MultiDimDecorator<T_DATA>::add( const DiscreteVariable& v ) {
    (( MultiDimContainer<T_DATA> * ) _content )->add( v );
  }

  // ==============================================================================
  /// listen to change in each recorded Instantiation. final method
  // ==============================================================================
  template<typename T_DATA> INLINE
  void MultiDimDecorator<T_DATA>::changeNotification
  ( Instantiation& i, const DiscreteVariable* const var,
    const Idx& oldval,const Idx& newval ) {
    (( MultiDimContainer<T_DATA> * ) _content )->changeNotification( i,var,oldval,
        newval );
  }

  // ==============================================================================
  /// listen to an assignment of a value in a Instantiation
  // ==============================================================================
  template<typename T_DATA> INLINE
  void MultiDimDecorator<T_DATA>::setChangeNotification( Instantiation& i ) {
    (( MultiDimContainer<T_DATA> * ) _content )->setChangeNotification( i );
  }

  // ==============================================================================
  /// listen to setFirst in each recorded Instantiation. final method.
  // ==============================================================================
  template<typename T_DATA> INLINE
  void MultiDimDecorator<T_DATA>::setFirstNotification( Instantiation& i ) {
    (( MultiDimContainer<T_DATA> * ) _content )->setFirstNotification( i );
  }

  // ==============================================================================
  /// listen to setLast in each recorded Instantiation. final method.
  // ==============================================================================
  template<typename T_DATA> INLINE
  void MultiDimDecorator<T_DATA>::setLastNotification( Instantiation& i ) {
    (( MultiDimContainer<T_DATA> * ) _content )->setLastNotification( i );
  }

  // ==============================================================================
  /// listen to increment in each recorded Instantiation. final method.
  // ==============================================================================
  template<typename T_DATA> INLINE
  void MultiDimDecorator<T_DATA>::setIncNotification( Instantiation& i ) {
    (( MultiDimContainer<T_DATA> * ) _content )->setIncNotification( i );
  }

  // ==============================================================================
  /// listen to increment in each recorded Instantiation. final method.
  // ==============================================================================
  template<typename T_DATA> INLINE
  void MultiDimDecorator<T_DATA>::setDecNotification( Instantiation& i ) {
    (( MultiDimContainer<T_DATA> * ) _content )->setDecNotification( i );
  }

  // ==============================================================================
  /// add a Instantiation as a slave of this
  // ==============================================================================
  template<typename T_DATA> INLINE
  bool MultiDimDecorator<T_DATA>::registerSlave( Instantiation& i ) {
    return (( MultiDimContainer<T_DATA> * ) _content )->registerSlave( i );
  }

  template<typename T_DATA> INLINE
  void MultiDimDecorator<T_DATA>::erase( const DiscreteVariable& d ) {
    (( MultiDimContainer<T_DATA> * ) _content )->erase( d );
  }

  template<typename T_DATA> INLINE
  const DiscreteVariable& MultiDimDecorator<T_DATA>::variable( const Idx i ) const {
    return (( MultiDimContainer<T_DATA> * ) _content )->variable( i );
  }

  template<typename T_DATA> INLINE
  Idx MultiDimDecorator<T_DATA>::pos( const DiscreteVariable& d ) const {
    return (( MultiDimContainer<T_DATA> * ) _content )->pos( d );
  }

  template<typename T_DATA> INLINE
  bool MultiDimDecorator<T_DATA>::contains( const DiscreteVariable& d ) const  {
    return (( MultiDimContainer<T_DATA> * ) _content )->contains( d );
  }

  template<typename T_DATA> INLINE
  bool MultiDimDecorator<T_DATA>::empty() const  {
    return (( MultiDimContainer<T_DATA> * ) _content )->empty();
  }

  template<typename T_DATA> INLINE
  bool MultiDimDecorator<T_DATA>::unregisterSlave( Instantiation& i ) {
    return (( MultiDimContainer<T_DATA> * ) _content )->unregisterSlave( i );
  }

  template<typename T_DATA> INLINE
  void MultiDimDecorator<T_DATA>::fill( const T_DATA& d )  const {
    (( MultiDimContainer<T_DATA> * ) _content )->fill( d );
  }

  // ==============================================================================
  /// notification modification on vars to all Instantiation listeners.
  // ==============================================================================
  template<typename T_DATA> INLINE
  void MultiDimDecorator<T_DATA>::notifyChange() const {
    /*( (MultiDimContainer<T_DATA> *) _content)->notifyChange();*/
    GUM_ERROR( OperationNotAllowed,"Not implemented yet" );
  }

  // ==============================================================================
  /// give a const ref to the sequence of DiscreteVariable*. final method.
  // ==============================================================================
  template<typename T_DATA> INLINE
  const Sequence<const DiscreteVariable *>&
  MultiDimDecorator<T_DATA>::variablesSequence() const  {
    return (( MultiDimContainer<T_DATA> * ) _content )->variablesSequence();
  }

  // ==============================================================================
  /// get the nbr of vars in the sequence. final method
  // ==============================================================================
  template<typename T_DATA> INLINE
  Idx MultiDimDecorator<T_DATA>::nbrDim() const  {
    return (( MultiDimContainer<T_DATA> * ) _content )->nbrDim();
  }

  /// In order to insure the deref. for decorators, we need to virtualize the
  /// access to master pointer
  template<typename T_DATA> INLINE
  MultiDimAdressable& MultiDimDecorator<T_DATA>:: getMasterRef( void ) {
    return *_content;
  }

  /// In order to insure the deref. for decorators, we need to virtualize the
  /// access to master pointer
  template<typename T_DATA> INLINE
  const MultiDimAdressable& MultiDimDecorator<T_DATA>:: getMasterRef( void )
  const {
    return *_content;
  }

  /// protected access to _content
  template<typename T_DATA> INLINE
  MultiDimImplementation<T_DATA>* MultiDimDecorator<T_DATA>::getContent() {
    return _content;
  }

  /// protected access to _content
  template<typename T_DATA> INLINE
  const MultiDimImplementation<T_DATA>* MultiDimDecorator<T_DATA>::getContent() const {
    return _content;
  }


  /// string representation of internal data about i in this.
  template<typename T_DATA> INLINE
  const std::string MultiDimDecorator<T_DATA>::toString( const Instantiation *i ) const {
    return _content->toString( i );
  }

  /// string representation of this.
  template<typename T_DATA> INLINE
  const std::string MultiDimDecorator<T_DATA>::toString( ) const {
    return _content->toString( );
  }

  template<typename T_DATA> INLINE
  void MultiDimDecorator<T_DATA>::beginMultipleChanges( void ) {
    (( MultiDimContainer<T_DATA> * ) _content )->beginMultipleChanges();
  }

  template<typename T_DATA> INLINE
  void MultiDimDecorator<T_DATA>::endMultipleChanges( void ) {
    (( MultiDimContainer<T_DATA> * ) _content )->endMultipleChanges();
  }

  /// Perform an homothety on a multiDim container
  template<typename T_DATA> INLINE
  void MultiDimDecorator<T_DATA>::homothetic
  ( const T_DATA alpha, T_DATA( *mul )( const T_DATA,const T_DATA ) ) {
    Instantiation i( this );
    i.setFirst();

    while ( ! i.end() ) {
      set( i ,mul( alpha,get( i ) ) );
      ++i;
    }
  }

  /// iterate add on each element of a multiDim container
  template<typename T_DATA> INLINE
  T_DATA  MultiDimDecorator<T_DATA>::fold
  ( T_DATA( *add )( const T_DATA,const T_DATA ) ) const {
    Instantiation i( this );
    T_DATA res;
    i.setFirst();res=get( i );++i;

    while ( ! i.end() ) {
      res=add( res,get( i ) );
      ++i;
    }

    return res;
  }


} /* namespace gum */

