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


namespace gum {
  // ==============================================================================
  /// Default constructor: creates an empty null dimensional matrix
  // ==============================================================================
  template<typename T_DATA>
  MultiDimWithOffset<T_DATA>::MultiDimWithOffset() : MultiDimImplementation<T_DATA>() {
    // for debugging purposes
    GUM_CONSTRUCTOR( MultiDimWithOffset );
  }

  // ==============================================================================
  /// copy constructor
  // ==============================================================================
  template<typename T_DATA>
  MultiDimWithOffset<T_DATA>::MultiDimWithOffset( const MultiDimWithOffset<T_DATA>& from ) :
      MultiDimImplementation<T_DATA> ( from ), _gaps( from._gaps ) {
    // for debugging purposes
    GUM_CONS_CPY( MultiDimWithOffset );
  }

  // ==============================================================================
  /// destructor
  // ==============================================================================
  template<typename T_DATA>
  MultiDimWithOffset<T_DATA>::~MultiDimWithOffset() {
    // for debugging purposes
    GUM_DESTRUCTOR( MultiDimWithOffset );
    // no need to unregister all slaves as it will be done by MultiDimImplementation
  }

  // ==============================================================================
  /// add a new dimension, needed for updating the _offsets & _gaps
  // ==============================================================================
  template<typename T_DATA>
  INLINE
  void MultiDimWithOffset<T_DATA>::add( const DiscreteVariable& v ) {
    Size lg=this->domainSize();

		if (lg>GUM_MAX_SIZE/v.domainSize()) GUM_ERROR(OutOfBounds,"Out of bounds !");

    MultiDimImplementation<T_DATA>::add( v );
    _gaps.insert( &v,lg );
  }

  // ==============================================================================
  /// removes a dimension, needed for updating the _offsets & _gaps
  // ==============================================================================
  template<typename T_DATA>
  INLINE
  void MultiDimWithOffset<T_DATA>::erase( const DiscreteVariable& v ) {
    Sequence<const DiscreteVariable *> variables=this->variablesSequence();
    Idx pos=variables[&v]; // throw a NotFound if necessary

    if ( variables.size() ==1 ) {
      _gaps.clear();
    } else {
      // update the _gaps
      Size v_size = v.domainSize();
      _gaps.erase( variables[pos] );

      for ( Idx i = pos+1; i < variables.size(); ++i ) {
        _gaps[variables[i]] /= v_size;
      }
    }

    MultiDimImplementation<T_DATA>::erase( v );
  }

  // ==============================================================================
  /// listen to change in each recorded Instantiation.
  // ==============================================================================
  template<typename T_DATA>
  INLINE void
  MultiDimWithOffset<T_DATA>::changeNotification( Instantiation& i,
      const DiscreteVariable* const var,
      const Idx& oldval,
      const Idx& newval ) {
    GUM_ASSERT( _offsets.exists( &i ) );
    GUM_ASSERT( _offsets[&i] < this->domainSize() );
    GUM_ASSERT( newval < var->domainSize() );
    GUM_ASSERT( oldval < var->domainSize() );

    if ( newval >= oldval ) {
      _offsets[&i]+= _gaps[var]* ( newval - oldval );
      GUM_ASSERT( _offsets[&i] < this->domainSize() );
    } else {
      GUM_ASSERT( _offsets[&i] >= _gaps[var]* ( oldval - newval ) );
      _offsets[&i]-= _gaps[var]* ( oldval - newval );
    }
  }

  // ==============================================================================
  /// listen to an assignment of a value in a Instantiation
  // ==============================================================================
  template<typename T_DATA>
  INLINE void
  MultiDimWithOffset<T_DATA>::setChangeNotification( Instantiation& i ) {
    GUM_ASSERT( _offsets.exists( &i ) );
    _offsets[&i] = _getOffs( i );
  }

  // ==============================================================================
  /// listen to setFirst in each recorded Instantiation.
  // ==============================================================================
  template<typename T_DATA>
  INLINE
  void MultiDimWithOffset<T_DATA>::setFirstNotification( Instantiation& i ) {
    GUM_ASSERT( _offsets.exists( &i ) );
    _offsets[&i]=0;
  }

  // ==============================================================================
  /// listen to setLast in each recorded Instantiation.
  // ==============================================================================
  template<typename T_DATA>
  INLINE
  void MultiDimWithOffset<T_DATA>::setLastNotification( Instantiation& i ) {
    GUM_ASSERT( _offsets.exists( &i ) );
    _offsets[&i]=this->domainSize()-1;
  }

  // ==============================================================================
  /// listen to increment in each recorded Instantiation.
  // ==============================================================================
  template<typename T_DATA>
  INLINE
  void MultiDimWithOffset<T_DATA>::setIncNotification( Instantiation& i ) {
    GUM_ASSERT( _offsets.exists( &i ) );
    GUM_ASSERT( _offsets[&i]!=this->domainSize()-1 );
    ++_offsets[&i];
  }

  // ==============================================================================
  /// listen to increment in each recorded Instantiation.
  // ==============================================================================
  template<typename T_DATA>
  INLINE
  void MultiDimWithOffset<T_DATA>::setDecNotification( Instantiation& i ) {
    GUM_ASSERT( _offsets.exists( &i ) );
    GUM_ASSERT( _offsets[&i]!=0 );
    --_offsets[&i];
  }

  // ==============================================================================
  /// add a Instantiation as a slave
  // ==============================================================================
  template<typename T_DATA>
  INLINE
  bool MultiDimWithOffset<T_DATA>::registerSlave( Instantiation& i ) {
    if ( MultiDimImplementation<T_DATA>::registerSlave( i ) ) {
      GUM_ASSERT( ! _offsets.exists( &i ) );
      _offsets.insert( &i, _getOffs( i ) );
      return true;
    }

    return false;
  }

  // ==============================================================================
  /// remove a registered slave instantiation
  // ==============================================================================
  template<typename T_DATA>
  INLINE
  bool MultiDimWithOffset<T_DATA>::unregisterSlave( Instantiation& i ) {
    MultiDimImplementation<T_DATA>::unregisterSlave( i );
    _offsets.erase( &i );
    return true;
  }

  // ============================================================================
  /// Compute the offset of a Instantiation
  /** If the instantiation is not fully compatible with the MultiDimWithOffset,
   * no exception thrown
   * but 0 is assumed for dimensions not present in the instantiation.
   * for instance : M<<a<<b<<c; with i=b:1|c:2|d:1 then M._getOffs(i) give the
   * offset of a:0|b:1|c:2.
   */
  // ============================================================================
  template<typename T_DATA> INLINE
  Size MultiDimWithOffset<T_DATA>::_getOffs( const Instantiation &i ) const {
    Idx off = 0;

    for ( HashTableIterator<const DiscreteVariable *,Size> iter=_gaps.begin();
          iter != _gaps.end(); ++iter )
      if ( i.contains( iter.key() ) ) off += *iter * i.val( iter.key() );

    return off;
  }

  // ============================================================================
  /// For a given indice of a value in the vector _values, this method computes
  /// the corresponding instantiation
  /**
   * @param result the result of this methods, we assume that the given
   * instantiation already contains all the variables
   *               contained in the multidimarray (if V is the set of variables
   *               of this tab, V must be a subset of variables in
   *               result or the exact set)
   * @param indice indice in the vector _values
   */
  // ============================================================================
  template<typename T_DATA>
  INLINE
  void MultiDimWithOffset<T_DATA>::_computeInstantiationValue( Instantiation& result,Size indice ) const {
    for ( Idx i = 0; i < this->nbrDim(); ++i ) {
      const DiscreteVariable& var = this->variable( i );
      const Idx domainSize = var.domainSize();
      result.chgVal( var, indice % domainSize );
      indice = indice / domainSize;
    }

    GUM_ASSERT( indice == 0 );
  }

  /// string representation of internal data about i in this.
  template<typename T_DATA> INLINE
  const std::string MultiDimWithOffset<T_DATA>::toString( const Instantiation *i ) const {
    if ( i->isMaster( this ) ) {
      std::stringstream s;
      s<<_offsets[i];
      std::string res;
      s >> res;
      return res;
    } else {
      return "--";
    }
  }


  template<typename T_DATA> INLINE
  Size MultiDimWithOffset<T_DATA>::toOffset( const Instantiation& i ) const {
    return _getOffs(i);
  }

  /// set the Instantiation to the values corresponding to the offset (in this array)
  template<typename T_DATA> INLINE
  Instantiation& MultiDimWithOffset<T_DATA>::fromOffset( Instantiation& i,Size offset ) const {
    this->_computeInstantiationValue(i,offset);
		return i;
	}

} /* namespace gum */

