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

#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {


  // ==============================================================================
  /// Default constructor
  // ==============================================================================
  template<typename T_DATA> INLINE
  MultiDimImplementation<T_DATA>::MultiDimImplementation():
    MultiDimContainer<T_DATA>(),__vars(),__slaveInstantiations() {
    GUM_CONSTRUCTOR( MultiDimImplementation );
    __internalChangeMethod=DIRECT_CHANGE;
    __internalChangeState=NO_CHANGE;
    __domainSize=1;
  }

  // ==============================================================================
  /// Copy constructor
  // ==============================================================================
  template<typename T_DATA> INLINE
  MultiDimImplementation<T_DATA>::MultiDimImplementation
  ( const MultiDimImplementation<T_DATA>& from ) :
    MultiDimContainer<T_DATA>( from ),__vars( from.__vars ),
    __internalChangeMethod( from.__internalChangeMethod ),
    __internalChangeState( from.__internalChangeState ),
    __domainSize( from.__domainSize ) {
    GUM_CONS_CPY( MultiDimImplementation );
    GUM_ASSERT( ! this->_isCommitNeeded() );
  }

  // ==============================================================================
  /// destructor
  // ==============================================================================
  template<typename T_DATA> INLINE
  MultiDimImplementation<T_DATA>::~MultiDimImplementation() {
    GUM_DESTRUCTOR( MultiDimImplementation );
    // unregister all remaining slave instantiations

    for ( List<Instantiation*>::iterator iter = __slaveInstantiations.begin();
          iter != __slaveInstantiations.end(); ++iter )
      ( *iter )->forgetMaster();
  }

  // ==============================================================================
  /// add a new var to the sequence of __vars.
  // ==============================================================================
  template<typename T_DATA> INLINE
  void MultiDimImplementation<T_DATA>::add( const DiscreteVariable& v ) {
    // check if the variable already belongs to the tuple of variables
    // of the Instantiation
    if ( __vars.exists( &v ) )
      GUM_ERROR( DuplicateElement,"Var already exists in this instantiation" );

    __domainSize*=v.domainSize();

    __vars.insert( &v );

    // informs all the slaves that they have to update themselves
    for ( List<Instantiation*>::iterator iter = __slaveInstantiations.begin();
          iter != __slaveInstantiations.end(); ++iter ) {
      ( *iter )->addWithMaster( this, v );
    }

    if ( _isInMultipleChangeMethod() ) __setNotCommitedChange();
  }

  // ==============================================================================
  /// removes a var from the variables of the multidimensional matrix
  // ==============================================================================
  template<typename T_DATA> INLINE
  void MultiDimImplementation<T_DATA>::erase( const DiscreteVariable& v ) {
    // check that the variable does actually belong to the MultiDimImplementation
    if ( ! __vars.exists( &v ) )
      GUM_ERROR( NotFound,"Var does not exist in this instantiation" );

    __domainSize/=v.domainSize();

    __vars.erase( &v );

    // informs all the slaves that they have to update themselves
    for ( List<Instantiation*>::iterator iter = __slaveInstantiations.begin();
          iter != __slaveInstantiations.end(); ++iter ) {
      ( *iter )->eraseWithMaster( this, v );
    }

    if ( _isInMultipleChangeMethod() ) __setNotCommitedChange();
  }

  // ==============================================================================
  /// adds a new var to the sequence of __vars
  /// @todo this function should be declared somewhere?
  // ==============================================================================
  template<typename T_DATA> INLINE
  MultiDimImplementation<T_DATA>& operator<<( MultiDimImplementation<T_DATA>& array,
                                              const DiscreteVariable& v ) {
    array.add( v );
    return array;
  }

  // ==============================================================================
  /// add a Instantiation to the list of slave instantiations
  // ==============================================================================
  template<typename T_DATA> INLINE
  bool MultiDimImplementation<T_DATA>::registerSlave( Instantiation& i ) {
    // check that the Instantiation has the same variables as this
    if ( i.nbrDim() != __vars.size() )
      return false;

    for ( Sequence<const DiscreteVariable *>::iterator iter = __vars.begin();
          iter != __vars.end(); ++iter )
      if ( ! i.contains( *iter ) )
        return false;

    i.synchronizeWithMaster( this );

    __slaveInstantiations+=( &i );

    return true;
  }

  // ==============================================================================
  /// removes a Instantiation from the list of slave instantiations
  // ==============================================================================
  template<typename T_DATA> INLINE
  bool MultiDimImplementation<T_DATA>::unregisterSlave( Instantiation& slave ) {
    __slaveInstantiations.eraseByVal( &slave );
    // TODO This method should return true? Why not use a void instead?
    return true;
  }

  template<typename T_DATA> INLINE
  Idx MultiDimImplementation<T_DATA>::nbrDim() const  {
    return __vars.size();
  }

  template<typename T_DATA> INLINE
  Size MultiDimImplementation<T_DATA>::domainSize() const  {
    return __domainSize;
  }

  template<typename T_DATA> INLINE
  const DiscreteVariable&
  MultiDimImplementation<T_DATA>::variable( const Idx i ) const  {
    return *( __vars.atPos( i ) );
  }

  template<typename T_DATA> INLINE
  Idx MultiDimImplementation<T_DATA>::pos( const DiscreteVariable& v ) const {
    return __vars[&v];
  }

  template<typename T_DATA> INLINE
  bool MultiDimImplementation<T_DATA>::contains( const DiscreteVariable& v ) const {
    return __vars.exists( &v );
  }

  // ==============================================================================
  /// returns a const ref to the sequence of DiscreteVariable*
  // ==============================================================================
  template<typename T_DATA> INLINE
  const Sequence<const DiscreteVariable *>&
  MultiDimImplementation<T_DATA>::variablesSequence( void ) const  {
    return __vars;
  }

  /// is this empty ?
  template<typename T_DATA> INLINE
  bool MultiDimImplementation<T_DATA>::empty() const  {
    GUM_ASSERT( ! this->_isCommitNeeded() );
    return __vars.empty();
  }

  template<typename T_DATA> INLINE
  MultiDimAdressable& MultiDimImplementation<T_DATA>::getMasterRef() {
    return *this;
  }

  template<typename T_DATA> INLINE
  const MultiDimAdressable& MultiDimImplementation<T_DATA>::getMasterRef() const {
    return *this;
  }

  template<typename T_DATA> INLINE
  void MultiDimImplementation<T_DATA>::beginMultipleChanges( void ) {
    __internalChangeMethod=MULTIPLE_CHANGE;
  }

  template<typename T_DATA> INLINE
  void MultiDimImplementation<T_DATA>::endMultipleChanges( void ) {
    if ( __internalChangeState==NOT_COMMITTED_CHANGE ) {
      _commitMultipleChanges();
      __internalChangeState=NO_CHANGE;
    }

    __internalChangeMethod=DIRECT_CHANGE;
  }

  template<typename T_DATA> INLINE
  void MultiDimImplementation<T_DATA>::_commitMultipleChanges( void ) {
    // empty!
  }

  /// get the actual change method of *this
  template<typename T_DATA> INLINE
  bool MultiDimImplementation<T_DATA>::_isInMultipleChangeMethod() const {
    return ( __internalChangeMethod==MULTIPLE_CHANGE );
  }

  /// get the actual state of *this
  template<typename T_DATA> INLINE
  bool MultiDimImplementation<T_DATA>::_isCommitNeeded() const {
    return ( __internalChangeState==NOT_COMMITTED_CHANGE );
  }

  /// Returns a constant reference over the list of slaved instantiations.
  template<typename T_DATA> INLINE
  const List<Instantiation*>&
  MultiDimImplementation<T_DATA>::_slaves() const {
    return __slaveInstantiations;
  }

  /// get the actual state of *this
  template<typename T_DATA> INLINE
  void MultiDimImplementation<T_DATA>::__setNotCommitedChange() {
    __internalChangeState=NOT_COMMITTED_CHANGE;
  }

  /// get the actual state of *this
  template<typename T_DATA> INLINE
  float MultiDimImplementation<T_DATA>::compressionRate() const {
    return ((float)1)-(float)realSize()/(float)domainSize();
  }


  // ==============================================================================
  /// for friendly displaying the content of the array
  // ==============================================================================
  template<typename T_DATA> INLINE
  std::ostream& operator<< ( std::ostream& out,
                             const MultiDimImplementation<T_DATA>& array ) {
    return out << static_cast<const MultiDimContainer<T_DATA>&>( array );
  }

} /* namespace gum */

#endif // DOXYGEN_SHOULD_SKIP_THIS
