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
  MultiDimSparse<T_DATA>::MultiDimSparse( const T_DATA& default_value ) : MultiDimWithOffset<T_DATA>(),_default( default_value ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( MultiDimSparse );
  }

  // ==============================================================================
  /// copy constructor
  // ==============================================================================
  template<typename T_DATA>
  MultiDimSparse<T_DATA>::MultiDimSparse( const MultiDimSparse<T_DATA>& from ) :
      MultiDimWithOffset<T_DATA> ( from ), _params( from._params ),_default( from._default ) {
    // for debugging purposes
    GUM_CONS_CPY( MultiDimSparse );
  }

  // ==============================================================================
  /// destructor
  // ==============================================================================
  template<typename T_DATA>
  MultiDimSparse<T_DATA>::~MultiDimSparse() {
    // for debugging purposes
    GUM_DESTRUCTOR( MultiDimSparse );
    // no need to unregister all slaves as it will be done by MultiDimWithOffset
  }

  // ==============================================================================
  /// data access operator
  // ==============================================================================
  template<typename T_DATA> INLINE
  T_DATA MultiDimSparse<T_DATA>::get( const Instantiation& i ) const {
    Size key;

    if ( i.isMaster( this ) ) {
      key=this->_offsets[&i];
    } else {
      key=this->_getOffs( i );
    }

    return _params.exists( key )?_params[key]:_default;
  }

  template<typename T_DATA> INLINE
  void MultiDimSparse<T_DATA>::set( const Instantiation& i,const T_DATA& value ) const {
    Size key;

    if ( i.isMaster( this ) ) {
      key=this->_offsets[&i];
    } else {
      key=this->_getOffs( i );
    }

    if ( value==_default ) {
      _params.reset( key );
    } else {
      _params.set( key,value );
    }
  }

  // ==============================================================================
  /// add a new dimension, needed for updating the _offsets & _gaps
  // ==============================================================================
  template<typename T_DATA>
  INLINE
  void MultiDimSparse<T_DATA>::add( const DiscreteVariable& v ) {
    MultiDimWithOffset<T_DATA>::add( v );

    //@todo : anything better than that !
    fill( _default );
  }

  // ==============================================================================
  /// removes a dimension, needed for updating the _offsets & _gaps
  // ==============================================================================
  template<typename T_DATA>
  INLINE
  void MultiDimSparse<T_DATA>::erase( const DiscreteVariable& v ) {
    MultiDimWithOffset<T_DATA>::erase( v );

    //@todo : anything better than that !
    fill( _default );
  }

  /// synchronise content after MultipleChanges
  template<typename T_DATA>
  INLINE
  void MultiDimSparse<T_DATA>::_commitMultipleChanges( void ) {
    //@todo : anything better than that !
    fill( _default );
  }

  // fill the array with the arg
  template<typename T_DATA>
  INLINE
  void MultiDimSparse<T_DATA>::fill( const T_DATA& d )  const {
    _params.clear();
    _default=d;
  }


  template<typename T_DATA> INLINE
  Size MultiDimSparse<T_DATA>::realSize( void ) const {
    return _params.size();
  }

  template<typename T_DATA> INLINE
  MultiDimContainer<T_DATA>* MultiDimSparse<T_DATA>::newFactory() const {
    return new MultiDimSparse<T_DATA>(_default);
  }


  // returns the name of the implementation
  template<typename T_DATA> INLINE
  const std::string& MultiDimSparse<T_DATA>::name () const {
    static const std::string str = "MultiDimSparse";
    return str;
  }

  template<typename T_DATA> INLINE
  void MultiDimSparse<T_DATA>::_swap(const DiscreteVariable* x,
                                     const DiscreteVariable* y)
  {
    MultiDimImplementation<T_DATA>::_swap(x,y);
  }

} /* namespace gum */

