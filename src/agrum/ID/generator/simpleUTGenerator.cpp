
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/** @file
 * @brief Source implementation of SimpleUTGenerator.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN(@LIP6)
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
  void SimpleUTGenerator::generateUT(const Idx&                varId,
                                     const Potential< float >& ut) {
    std::vector< float > v;

    for (Size i = 0; i < ut.domainSize(); ++i)
      v.push_back((float)rand() * 1000);

    ut.populate(v);
  }

  // Generates a UT using doubles.
  // @param varID The variable id of the UT owner.
  // @param ut A reference on the UT to fill.
  void SimpleUTGenerator::generateUT(const Idx&                 varId,
                                     const Potential< double >& ut) {
    std::vector< double > v;

    for (Size i = 0; i < ut.domainSize(); ++i)
      v.push_back((double)rand() * 1000);

    ut.populate(v);
  }

} /* namespace gum */
