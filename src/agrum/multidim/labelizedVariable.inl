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
 * @brief Base class for labelized discrete random variables
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 */
#include <string>
#include <sstream>
#include <ostream>
#include <agrum/core/debug.h>
#include <agrum/core/exceptions.h>
#include <agrum/multidim/discreteVariable.h>
#include <agrum/core/hashTable.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {


  // ==============================================================================
  /// erase all the labels
  // ==============================================================================
  INLINE void LabelizedVariable::eraseLabels( void ) {
    zeLabels.clear();
//     raiseDomainSizeChanged();
  }

  // ==============================================================================
  /// copies the content of aLDRV
  // ==============================================================================
  INLINE void LabelizedVariable::_copy( const LabelizedVariable& aLDRV ) {
    DiscreteVariable::_copy( aLDRV );
    zeLabels.clear();
    zeLabels=aLDRV.labels();
//     raiseDomainSizeChanged();
  }

  // ==============================================================================
  /// returns the set of labels of the variable
  // ==============================================================================
  INLINE const Sequence<std::string>& LabelizedVariable::labels() const {
    return zeLabels;
  }

  // ==============================================================================
  /// add a label with a new index (we assume that we will NEVER remove a label)
  // ==============================================================================
  INLINE LabelizedVariable& LabelizedVariable::addLabel( const std::string aLabel ) {
    zeLabels.insert( aLabel );
//     raiseDomainSizeChanged();

    return *this;
  }

  // ==============================================================================
  // Default constructor
  // ==============================================================================
  INLINE LabelizedVariable::LabelizedVariable
  ( const std::string&  aName,const std::string&  aDesc,const int nbrLabel ) :
    DiscreteVariable( aName, aDesc ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( LabelizedVariable );

    for ( int i=0; i<nbrLabel; ++i ) {
      std::ostringstream oss;
      oss << i;
      addLabel( oss.str() );
    }
  }

  // ==============================================================================
  /// Copy constructor
  // ==============================================================================
  INLINE
  LabelizedVariable::LabelizedVariable( const LabelizedVariable& aLDRV ) :
    DiscreteVariable( aLDRV ),zeLabels( aLDRV.labels() ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( LabelizedVariable );
  }

  // ==============================================================================
  /// destructor
  // ==============================================================================
  INLINE LabelizedVariable::~LabelizedVariable() {
    eraseLabels();
    GUM_DESTRUCTOR( LabelizedVariable );
  }

  INLINE
  DiscreteVariable*
  LabelizedVariable::copyFactory() const {
    LabelizedVariable* varPtr = new LabelizedVariable( *this );
    return ( DiscreteVariable* ) varPtr;
  }

  // ==============================================================================
  /// copy operator
  // ==============================================================================
  INLINE const LabelizedVariable& LabelizedVariable::operator=
  ( const LabelizedVariable& aLDRV ) {
    // avoid self assignment
    if ( &aLDRV != this ) {
      _copy( aLDRV );
    }

    return *this;
  }

  // ==============================================================================
  /// indicates whether the variable already has the label passed in argument
  // ==============================================================================
  INLINE bool LabelizedVariable::isLabel( const std::string & aLabel ) const {
    return zeLabels.exists( aLabel );
  }

  // ==============================================================================
  /// returns the ith label#ifndef DOXYGEN_SHOULD_SKIP_THIS

  // ==============================================================================
  INLINE const std::string LabelizedVariable::label( const Idx i ) const {
    return zeLabels.atPos( i );
  }

  // ==============================================================================
  /// returns the index of a given label
  // ==============================================================================
  INLINE Idx LabelizedVariable::operator[]( const std::string& aLabel ) const {
    try {
      return zeLabels[aLabel];
    } catch ( ... ) {
      GUM_ERROR( OutOfBounds, "inexisting label" );
    }
  }

  // ==============================================================================
  /// returns the size of the random discrete variable domain
  // ==============================================================================
  INLINE Size LabelizedVariable::domainSize() const {
    return zeLabels.size();
  }


} /* namespace gum */
  
  
#endif /* DOXYGEN SHOULD SKIP THIS */
