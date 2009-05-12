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

#ifndef DUMMY_DISCRETE_VAR_H
#define DUMMY_DISCRETE_VAR_H

#include <string>

#include <agrum/multidim/discreteVariable.h>
#include <agrum/core/exceptions.h>
/**
 * Dummy variables.
 */

class DummyDiscreteVar: public gum::DiscreteVariable {
  public:
    DummyDiscreteVar( const std::string &aName, const std::string &aDesc );

    DummyDiscreteVar( const DummyDiscreteVar &source );

    virtual ~DummyDiscreteVar();

    virtual gum::DiscreteVariable* copyFactory() const;

    virtual bool empty() const;

		///  @throws gum::OutOfBounds
    virtual const std::string label( gum::Idx indice ) const;

    virtual gum::Size domainSize() const;

    void setDomainSize( const gum::Idx &size );

    virtual gum::Idx operator[]( const std::string& label ) const;

  private:
    gum::Idx __domainSize;

};

#endif  /* DUMMY_DISCRETE_VAR_H */

