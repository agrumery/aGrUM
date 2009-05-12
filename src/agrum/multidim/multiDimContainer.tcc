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
  /// Default constructor
  // ==============================================================================
  template<typename T_DATA> INLINE
  MultiDimContainer<T_DATA>::MultiDimContainer() : MultiDimAdressable() {
    GUM_CONSTRUCTOR( MultiDimContainer );
  }

  // ==============================================================================
  /// Copy constructor
  // ==============================================================================
  template<typename T_DATA>
  INLINE
  MultiDimContainer<T_DATA>::MultiDimContainer
  ( const MultiDimContainer<T_DATA>& from ) : MultiDimAdressable( from ) {
    GUM_CONS_CPY( MultiDimContainer );
  }

  // ==============================================================================
  /// destructor
  // ==============================================================================
  template<typename T_DATA>
  INLINE
  MultiDimContainer<T_DATA>::~MultiDimContainer() {
    GUM_DESTRUCTOR( MultiDimContainer );
  }

  // ==============================================================================
  /// an [] operator using a Instantiation as argument
  // ==============================================================================
  template<typename T_DATA>
  INLINE
  T_DATA MultiDimContainer<T_DATA>::operator[]( const Instantiation& i ) const {
    return get( i );
  }

  // ==============================================================================
  /// an [] operator using a Instantiation as argument
  // ==============================================================================
  template<typename T_DATA>
  INLINE void MultiDimContainer<T_DATA>::set( const Instantiation& i,const T_DATA& value ) const {
    _get( i )=value;
  }

  // ==============================================================================
  /// an [] operator using a Instantiation as argument
  // ==============================================================================
  template<typename T_DATA>
  INLINE T_DATA MultiDimContainer<T_DATA>::get( const Instantiation& i ) const {
    return _get( i );
  }

  // ==============================================================================
  /// display the content of an array
  // ==============================================================================
  template<typename T_DATA>
  const std::string MultiDimContainer<T_DATA>::toString() const {
    // we create a new instantiation and iterate over it to display the whole
    // content of the array
    std::stringstream ss;
    Instantiation myinst( const_cast<MultiDimContainer *>( this ) );

    bool first=true;

    for ( myinst.setFirst(); ! myinst.end(); ++myinst ) {
      if ( !first ) {
        ss<<" /";
      }

      first=false;

      ss << myinst << " :: " << get( myinst );
    }

    return ss.str();
  }

  // ==============================================================================
  /// automation fill with vector.
  // ==============================================================================
  template<typename T_DATA>
  void MultiDimContainer<T_DATA>::fillWith( const std::vector<T_DATA>& v ) const {
    if ( domainSize() != v.size() ) {
      GUM_ERROR( SizeError, "Sizes does not match in fillWith" );
    }

    Size cpt = 0;

    Instantiation i( *this );

    for ( i.setFirst();! i.end();++i, ++cpt )
      set( i, v[cpt] );
  }

  // ==============================================================================
  /// copyFrom
  // ==============================================================================
  template<typename T_DATA>
  void MultiDimContainer<T_DATA>::copyFrom
  ( const MultiDimContainer<T_DATA>& from ,Instantiation* p_i ) const {
    if ( from.domainSize()!=domainSize() )
      GUM_ERROR( OperationNotAllowed,"Domain size do not fit" );

    Instantiation i_dest( *this );

    Instantiation i_from( from );

    if ( p_i==( Instantiation * )0 ) {
      for ( i_dest.setFirst(),i_from.setFirst();! i_dest.end() ;
            ++i_dest,++i_from ) {
        set(i_dest,from[i_from]);
      }
    } else {
      for ( i_dest.setFirst(),i_from.setFirst();! i_dest.end() ;
            i_dest.incIn( *p_i ),++i_from ) {
        set(i_dest,from[i_from]);
      }
    }
  }

  // ==============================================================================
  /// copy
  // ==============================================================================
  template<typename T_DATA>
  void MultiDimContainer<T_DATA>::copy( const MultiDimContainer<T_DATA>& from ) {
    this->beginMultipleChanges();

    for ( Idx i=0;i<this->nbrDim();i++ ) {
      this->erase( this->variable( i ) );
    }

    for ( Idx i=0;i<from.nbrDim();i++ ) {
      this->add( from.variable( i ) );
    }

    this->endMultipleChanges();

    this->copyFrom( from );
  }

  // ==============================================================================
  /// display the content of an array
  // ==============================================================================
  template<typename T_DATA>
  std::ostream& operator<< ( std::ostream& out,
                             const MultiDimContainer<T_DATA>& array ) {
    out << array.toString();
    return out;
  }


} /* namespace gum */

