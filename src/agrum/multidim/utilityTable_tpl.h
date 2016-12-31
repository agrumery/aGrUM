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
/**
 * @file
 * @brief Implementation of the UtilityTable class.
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 * @author Jean-Philippe DUBUS
 */

namespace gum {

  template <typename GUM_SCALAR>
  INLINE UtilityTable<GUM_SCALAR>::UtilityTable()
      : MultiDimDecorator<GUM_SCALAR>( new MultiDimArray<GUM_SCALAR> ) {
    GUM_CONSTRUCTOR( UtilityTable );
  }

  template <typename GUM_SCALAR>
  INLINE UtilityTable<GUM_SCALAR>::UtilityTable(
      MultiDimImplementation<GUM_SCALAR>* aContent )
      : MultiDimDecorator<GUM_SCALAR>( aContent ) {
    GUM_CONSTRUCTOR( UtilityTable );
  }

  template <typename GUM_SCALAR>
  INLINE UtilityTable<GUM_SCALAR>::~UtilityTable() {
    GUM_DESTRUCTOR( UtilityTable );
  }

  template <typename GUM_SCALAR>
  INLINE
  UtilityTable<GUM_SCALAR>::UtilityTable( const UtilityTable<GUM_SCALAR>& toCopy )
      : MultiDimDecorator<GUM_SCALAR>(
            static_cast<MultiDimImplementation<GUM_SCALAR>*>(
                toCopy.content()->newFactory() ) ) {
    const Sequence<const DiscreteVariable*>& varSeq = toCopy.variablesSequence();

    for ( Sequence<const DiscreteVariable*>::iterator_safe iter =
              varSeq.beginSafe();
          iter != varSeq.endSafe();
          ++iter ) {
      this->add( **iter );
    }

    Instantiation i1( toCopy );
    Instantiation i2( *this );

    for ( i1.setFirst(), i2.setFirstIn( i1 ); !i1.end(); ++i1, i2.incIn( i1 ) ) {
      this->set( i2, toCopy[i1] );
    }

    GUM_CONS_CPY( UtilityTable );
  }

  template <typename GUM_SCALAR>
  INLINE UtilityTable<GUM_SCALAR>& UtilityTable<GUM_SCALAR>::
  operator=( const UtilityTable<GUM_SCALAR>& toCopy ) {
    GUM_ERROR( OperationNotAllowed,
               "No copy for UtilityTable : how to choose the implementation ?" );
    return *this;
  }

  template <typename GUM_SCALAR>
  INLINE UtilityTable<GUM_SCALAR>* UtilityTable<GUM_SCALAR>::newFactory() const {
    return new UtilityTable<GUM_SCALAR>(
        static_cast<MultiDimImplementation<GUM_SCALAR>*>(
            this->content()->newFactory() ) );
  }

  template <typename GUM_SCALAR>
  INLINE void UtilityTable<GUM_SCALAR>::_swap( const DiscreteVariable* x,
                                               const DiscreteVariable* y ) {
    MultiDimDecorator<GUM_SCALAR>::content()->swap( *x, *y );
  }

} /* namespace gum */
