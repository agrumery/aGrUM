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
#include <sstream>
#include <agrum/multidim/multiDimAdressable.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {


  void Instantiation::__init ( MultiDimAdressable *master ) {
    // for speed issues
    const Sequence<const DiscreteVariable *>& v = master->variablesSequence();
    __vars.resize ( v.size() );
    __vals.reserve ( v.size() );
    // fill the instantiation

    for ( Sequence<const DiscreteVariable *>::iterator iter = v.begin();
          iter != v.end(); ++iter ) {
      __add ( **iter );
    }

    if ( master ) actAsSlave ( master->getMasterRef() );
  }

  // ==============================================================================
  /// constructor for a Instantiation contained into a MultiDimInterface
  // ==============================================================================
  Instantiation::Instantiation ( MultiDimAdressable &d ) :
    __master ( 0 ), __overflow ( false ) {
    // for debugging purposes
    GUM_CONSTRUCTOR ( Instantiation );
    __init ( &d );
  }

  Instantiation::Instantiation ( const MultiDimAdressable &d ) :
    __master ( 0 ), __overflow ( false ) {
    // for debugging purposes
    GUM_CONSTRUCTOR ( Instantiation );
    __init ( const_cast<MultiDimAdressable *> ( &d ) );
  }

  // ==============================================================================
  /// constructor for a Instantiation contained into a MultiDimInterface
  // ==============================================================================
  Instantiation::Instantiation ( MultiDimAdressable *d ) :
    __master ( 0 ), __overflow ( false ) {
    // for debugging purposes
    GUM_CONSTRUCTOR ( Instantiation );

    if ( d ) __init ( d );
  }

  // ==============================================================================
  /// constructor for a Instantiation contained into a MultiDimInterface
  /** this constructor is needed in order to allow creation of Instantiation(this)
   * in MultiDimAdressable and below */
  // ==============================================================================
  Instantiation::Instantiation ( const MultiDimAdressable *const_d ) :
    __master ( 0 ), __overflow ( false ) {
    // for debugging purposes
    GUM_CONSTRUCTOR ( Instantiation );

    if ( const_d ) __init ( const_cast<MultiDimAdressable *> ( const_d ) );
  }

  // ==============================================================================
  /// copy constructor
  // ==============================================================================
  Instantiation::Instantiation ( const Instantiation &aI,
                                 const bool notifyMaster ) :
    MultiDimInterface(), __master ( 0 ), __overflow ( false ) {
    // for debugging purposes
    GUM_CONS_CPY ( Instantiation );
    // copy the content of aI
    __vars = aI.__vars;
    __vals = aI.__vals;
    __overflow = aI.__overflow;

    if ( aI.__master && notifyMaster ) actAsSlave ( *aI.__master );
  }

  // operator=
  Instantiation &Instantiation::operator= ( const Instantiation &aI ) {
    if ( __master ) {
      if ( ! aI.isMaster ( __master ) ) { // aI as the same master.
        if ( nbrDim() != aI.nbrDim() ) {
          GUM_ERROR ( OperationNotAllowed, "in slave Instantiation" );
        }

        for ( Idx i = 0; i < nbrDim(); i++ ) {
          if ( ( ! contains ( aI.variable ( i ) ) ) ||
               ( ! aI.contains ( variable ( i ) ) ) ) {
            GUM_ERROR ( OperationNotAllowed, "in slave Instantiation" );
          }
        }
      }

      chgValIn ( aI );
    } else {
      // copy the content of aI
      __vars = aI.__vars;
      __vals = aI.__vals;
      __overflow = aI.__overflow;

      if ( aI.__master ) actAsSlave ( *aI.__master );
    }

    return *this;
  }

  // ==============================================================================
  /// function is called by the master (if any) when changes arise in its vars list
  // ==============================================================================
  /*
    void Instantiation::changeDimCommand
    (const Sequence<const DiscreteVariable *>& v) {
    bool modif = false; // indicates whether we modified sequence vars
    // first we remove all the variables belonging to this but not to v
    for (Idx index = __vars.size() - 1; index < __vars.size(); --index)
    if (! v.exists (__vars[index])) {
    __erase (*(__vars[index]));
    modif = true;
    }
    // next, add the missing dimensions
    for (Sequence<const DiscreteVariable *>::iterator iter = v.begin();
    iter != v.end(); ++iter)
    if (! __vars.exists (v[iter])) {
    __add (*v[iter]);
    modif = true;
    }
    // if we are a slave of a master, then we should update in the latter the pointer
    // to the array's valued pointed to by the Instantiation
    if (isSlave() && modif)
    __master->changeNotification(*this);
    }
  */
  ///Gives a string version of a Instantiation
  std::string Instantiation::toString() const  {
    std::stringstream sstr;
    // check if the value of the instantiation is correct

    if ( __overflow ) {
      sstr << "<invalid>";
    }

    sstr << "<";

    Sequence<const DiscreteVariable *>::iterator iter = __vars.begin();

    if ( iter != __vars.end() ) {
      sstr << variable ( iter.pos() ).name() << ":" << val ( iter.pos() );
      ++iter;

      while ( iter != __vars.end() ) {
        sstr << "|" << variable ( iter.pos() ).name() << ":" << val ( iter.pos() );
        ++iter;
      }
    }

    sstr  << ">";

    return sstr.str();
  }

  /// give a Id value for Hamming distance
  Idx Instantiation::hamming() const  {
    Sequence<const DiscreteVariable *>::iterator iter = __vars.begin();
    Idx res = 0;

    do
      res += val ( iter.pos() );

    while ( ++iter != __vars.end() );

    return res;
  }

  // ==============================================================================
  /// an operator for user-friendly displaying the content of a Instantiation
  // ==============================================================================
  std::ostream &operator<< ( std::ostream &aStream,
                             const Instantiation &i ) {
    aStream << i.toString();
    return aStream;
  }


} /* namespace gum */


#endif    // DOXYGEN_SHOULD_SKIP_THIS

