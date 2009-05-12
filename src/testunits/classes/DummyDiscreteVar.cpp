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

#include <sstream>

#include "DummyDiscreteVar.h"

DummyDiscreteVar::DummyDiscreteVar( const std::string &aName, const std::string &aDesc ):
    gum::DiscreteVariable( aName, aDesc ), __domainSize( 2 ) {

}

DummyDiscreteVar::DummyDiscreteVar( const DummyDiscreteVar &source ):
    gum::DiscreteVariable( source ), __domainSize( source.__domainSize ) {

}

DummyDiscreteVar::~DummyDiscreteVar() {

}

gum::DiscreteVariable*
DummyDiscreteVar::copyFactory() const {
  return new DummyDiscreteVar( *this );
}

bool
DummyDiscreteVar::empty() const {
  return ( __domainSize == 0 );
}

const std::string
DummyDiscreteVar::label( gum::Idx indice ) const {
  if ( indice < __domainSize ) {
    std::ostringstream stm;
    stm << indice;

    return stm.str();
  } else {
    throw gum::OutOfBounds( "Out of bound!" );
  }
}

gum::Idx DummyDiscreteVar::operator[](const std::string& l) const {
  GUM_ERROR(gum::NotFound,"that's it");
}

gum::Size
DummyDiscreteVar::domainSize() const {
  return __domainSize;
}

void
DummyDiscreteVar::setDomainSize( const gum::Idx &size ) {
  __domainSize = size;
  raiseDomainSizeChanged();
}

