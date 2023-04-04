/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) et Christophe GONZALES(_at_AMU)
 * (_at_AMU) info_at_agrum_dot_org
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


/**
 * @file
 * @brief Implementation of the Class encapsulating computations of notions from Information Theory
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#include <agrum/tools/graphicalModels/algorithm/informationTheory.h>

#define INFORMATION_THEORY_TEMPLATE                                              \
  template < template < typename > class INFERENCE_ENGINE, typename GUM_SCALAR > \
    requires JointTargettable< INFERENCE_ENGINE< GUM_SCALAR > >

namespace gum {
  INFORMATION_THEORY_TEMPLATE
  InformationTheory< INFERENCE_ENGINE, GUM_SCALAR >::InformationTheory(
     INFERENCE_ENGINE< GUM_SCALAR >& engine,
     const gum::NodeSet&             X,
     const gum::NodeSet&             Y) {
    std::cout << "In Information Theory" << std::endl;
    GUM_CONSTRUCTOR(InformationTheory)
  }

#undef INFORMATION_THEORY_TEMPLATE
}   // namespace gum