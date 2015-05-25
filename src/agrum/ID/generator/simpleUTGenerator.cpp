/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
 * @brief Source implementation of SimpleUTGenerator.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 *
 */
#include <agrum/ID/generator/simpleUTGenerator.h>

namespace gum {

  // Default constructor.
  SimpleUTGenerator::SimpleUTGenerator() { GUM_CONSTRUCTOR(SimpleUTGenerator); }

  // Destructor.
  SimpleUTGenerator::~SimpleUTGenerator() { GUM_DESTRUCTOR(SimpleUTGenerator); }

  // Generates a UT using floats.
  // @param varID The variable id of the UT owner.
  // @param ut A reference on the UT to fill.
  void SimpleUTGenerator::generateUT(const Idx &varId,
                                     const UtilityTable<float> &ut) {
    std::vector<float> v;

    for (Size i = 0; i < ut.domainSize(); ++i)
      v.push_back((float)rand() * 1000);

    ut.fillWith(v);
  }

  // Generates a UT using doubles.
  // @param varID The variable id of the UT owner.
  // @param ut A reference on the UT to fill.
  void SimpleUTGenerator::generateUT(const Idx &varId,
                                     const UtilityTable<double> &ut) {
    std::vector<double> v;

    for (Size i = 0; i < ut.domainSize(); ++i)
      v.push_back((double)rand() * 1000);

    ut.fillWith(v);
  }

} /* namespace gum */
