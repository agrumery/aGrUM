/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Ariele-Paolo MAESANO  *
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
#include <agrum/multidim/multiDimAdressable.h>
#include <agrum/multidim/setInst.h>
#include <sstream>

#ifdef GUM_NO_INLINE
#include <agrum/multidim/setInst_inl.h>
#endif

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  void SetInst::__init( MultiDimAdressable* master ) {
    // for speed issues
    const Sequence<const DiscreteVariable*>& v = master->variablesSequence();
    __vars.resize( v.size() );
    __vals.reserve( v.size() );
    // fill the SetInst

    for ( const auto var : v )
      __add( *var );

    // if ( master ) actAsSlave( master->getMasterRef() );
  }

  // constructor for a SetInst contained into a MultiDimInterface

  SetInst::SetInst( MultiDimAdressable& d )
      : /*__master( 0 ),*/ __overflow( false ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( SetInst );
    __init( &d );
  }

  SetInst::SetInst( const MultiDimAdressable& d )
      : /*__master( 0 ),*/ __overflow( false ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( SetInst );
    __init( const_cast<MultiDimAdressable*>( &d ) );
  }

  // constructor for a SetInst contained into a MultiDimInterface

  SetInst::SetInst( MultiDimAdressable* d )
      : /*__master( 0 ),*/ __overflow( false ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( SetInst );

    if ( d ) __init( d );
  }

  // constructor for a SetInst contained into a MultiDimInterface
  /** this constructor is needed in order to allow creation of SetInst(this)
   * in MultiDimAdressable and below */

  SetInst::SetInst( const MultiDimAdressable* const_d )
      : /*__master( 0 ),*/ __overflow( false ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( SetInst );

    if ( const_d ) __init( const_cast<MultiDimAdressable*>( const_d ) );
  }

  // copy constructor

  SetInst::SetInst( const SetInst& aI )
      : /* MultiDimInterface(), __master( 0 ),*/ __overflow( false ) {
    // for debugging purposes
    GUM_CONS_CPY( SetInst );
    // copy the content of aI
    __vars = aI.__vars;
    __vals = aI.__vals;
    __overflow = aI.__overflow;

    // if ( aI.__master && notifyMaster ) actAsSlave( *aI.__master );
  }

  SetInst::SetInst( const Instantiation& aI )
      : /* MultiDimInterface(), __master( 0 ),*/ __overflow( false ) {
    // for debugging purposes
    GUM_CONS_CPY( SetInst );
    const Sequence<const DiscreteVariable*>& v = aI.variablesSequence();
    __vars.resize( v.size() );
    //__vals.reserve( v.size() );
    // fill the SetInst

    for ( const auto var : v ) {
      __add( *var );
      __vals[__vars.pos( var )] = ( 1 << ( aI.val( *var ) ) );
    }
  }

  // operator=
  SetInst& SetInst::operator=( const SetInst& aI ) {
    // copy the content of aI
    __vars = aI.__vars;
    __vals = aI.__vals;
    __overflow = aI.__overflow;

    return *this;
  }

  // Gives a string version of a SetInst
  std::string SetInst::toString() const {
    std::stringstream sstr;
    // check if the value of the SetInst is correct

    if ( __overflow ) {
      sstr << "<invalid>";
    }

    sstr << "<";

    Sequence<const DiscreteVariable*>::iterator_safe iter = __vars.begin();

    if ( iter != __vars.end() ) {
      std::stringstream sstr2;
      sstr2.str( "" );
      Size si = variable( iter.pos() ).domainSize();
      Size valb = vals( iter.pos() );

      while ( si-- != 0 ) {
        std::stringstream sstr4;
        sstr4 << ( valb & 1 ? "1" : "0" ) << sstr2.str();
        valb >>= 1;
        sstr2.str( "" );
        ;
        sstr2 << sstr4.str();
      }

      sstr << variable( iter.pos() ).name() << ":" << sstr2.str();
      ++iter;

      while ( iter != __vars.end() ) {
        std::stringstream sstr3;
        sstr3 << "";
        si = variable( iter.pos() ).domainSize();

        valb = vals( iter.pos() );

        while ( si-- != 0 ) {
          std::stringstream sstr4;

          sstr4 << ( valb & 1 ? "1" : "0" ) << sstr3.str();
          valb >>= 1;
          sstr3.str( "" );
          sstr3 << sstr4.str();
        }

        sstr << "|" << variable( iter.pos() ).name() << ":" << sstr3.str();
        ++iter;
      }
    }

    sstr << ">";

    return sstr.str();
  }

  // an operator for user-friendly displaying the content of a SetInst
  std::ostream& operator<<( std::ostream& aStream, const SetInst& i ) {
    aStream << i.toString();
    return aStream;
  }
  gum::SetInst& operator<<( gum::SetInst& inst, const gum::DiscreteVariable& i ) {
    inst.add( i );
    return inst;
  }
  gum::SetInst& operator>>( gum::SetInst& inst, const gum::DiscreteVariable& i ) {
    inst.erase( i );
    return inst;
  }

  void
  gum::SetInst::assign_values( gum::Bijection<const gum::DiscreteVariable*,
                                              const gum::DiscreteVariable*>& bij,
                               const gum::SetInst&                           i,
                               gum::SetInst&                                 j ) {
    try {
      for ( const auto var : i.variablesSequence() )
        j.chgVal( bij.second( var ), i.val( var ) );
    } catch ( gum::NotFound& ) {
      GUM_ERROR( gum::NotFound, "missing variable in bijection or SetInst" );
    }
  }

} /* namespace gum */

#endif  // DOXYGEN_SHOULD_SKIP_THIS
