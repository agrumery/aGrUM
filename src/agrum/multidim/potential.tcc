/**************************************************************************
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
/**
 * @file
 * @brief Implementation of the Potential class.
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 */

namespace gum {

/// move constructor

  template <typename GUM_SCALAR>
  INLINE Potential<GUM_SCALAR>::Potential( const Potential<GUM_SCALAR>&& from )
      : MultiDimDecorator<GUM_SCALAR>( std::forward<const MultiDimDecorator<GUM_SCALAR>&&>( from ) ) {
    GUM_CONS_MOV( Potential );
  }

  // Default constructor: creates an empty null dimensional matrix
  /*
   * choose a MultiDimArray<> as decorated implementation  */

  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR>::Potential()
      : MultiDimDecorator<GUM_SCALAR>( new MultiDimArray<GUM_SCALAR>() ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( Potential );
  }

  // Default constructor: creates an empty null dimensional matrix

  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR>::Potential(
      MultiDimImplementation<GUM_SCALAR>* aContent )
      : MultiDimDecorator<GUM_SCALAR>( aContent ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( Potential );
  }

  // copy constructor
  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR>::Potential( const Potential<GUM_SCALAR>& src )
      : Potential<GUM_SCALAR>( static_cast<MultiDimImplementation<GUM_SCALAR>*>(
                                   src.content()->newFactory() ),
                               *( src.content() ) ) {
    // for debugging purposes
    GUM_CONS_CPY( Potential );
  }

  // complex copy constructor : we choose the implementation
  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR>::Potential(
      MultiDimImplementation<GUM_SCALAR>* aContent,
      const MultiDimContainer<GUM_SCALAR>& src )
      : MultiDimDecorator<GUM_SCALAR>( aContent ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( Potential );

    if ( !src.empty() ) {
      this->beginMultipleChanges();

      for ( Idx i = 0; i < src.variablesSequence().size(); i++ ) {
        this->add( *( src.variablesSequence()[i] ) );
      }

      this->endMultipleChanges();

      MultiDimDecorator<GUM_SCALAR>::content()->copyFrom(
          dynamic_cast<const MultiDimContainer<GUM_SCALAR>&>(
              src.getMasterRef() ) );
    }
  }

  // operator =
  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR>& Potential<GUM_SCALAR>::
  operator=( const Potential<GUM_SCALAR>& src ) {
    MultiDimDecorator<GUM_SCALAR>::operator=(src);
    return *this;
  }

  // destructor

  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR>::~Potential() {
    // for debugging purposes
    GUM_DESTRUCTOR( Potential );
  }

  template <typename GUM_SCALAR>
  INLINE Potential<GUM_SCALAR>* Potential<GUM_SCALAR>::newFactory() const {
    return new Potential<GUM_SCALAR>(
        static_cast<MultiDimImplementation<GUM_SCALAR>*>(
            this->content()->newFactory() ) );
  }

  template <typename GUM_SCALAR>
  INLINE void Potential<GUM_SCALAR>::_swap( const DiscreteVariable* x,
                                            const DiscreteVariable* y ) {
    MultiDimDecorator<GUM_SCALAR>::content()->swap( *x, *y );
  }

  // string representation of this.
  template <typename GUM_SCALAR>
  INLINE const std::string Potential<GUM_SCALAR>::toString() const {
    return MultiDimDecorator<GUM_SCALAR>::content()->toString();
  }

  // sum of all elements in this
  template <typename GUM_SCALAR>
  INLINE const GUM_SCALAR Potential<GUM_SCALAR>::sum() const {
    Instantiation i( this->_content );
    GUM_SCALAR s = (GUM_SCALAR)0;

    for ( i.setFirst(); !i.end(); ++i )
      s += this->get( i );

    return s;
  }

  // normalisation of this
  // do nothing is sum is 0
  template <typename GUM_SCALAR>
  INLINE Potential<GUM_SCALAR>& Potential<GUM_SCALAR>::normalize() const {
    GUM_SCALAR s = sum();

    if ( s != (GUM_SCALAR)0 ) {
      Instantiation i( this->_content );

      for ( i.setFirst(); !i.end(); ++i )
        this->set( i, this->get( i ) / s );
    }

    // a const method should return a const ref BUT WE NEED t return a non const
    // ref
    return const_cast<Potential<GUM_SCALAR>&>( *this );
  }

} /* namespace gum */
