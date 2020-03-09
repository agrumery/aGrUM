
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @brief the different types of apriori
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_A_PRIORI_TYPES_H
#define GUM_LEARNING_A_PRIORI_TYPES_H

#include <string>

namespace gum {

  namespace learning {

    struct AprioriDirichletType {
      static const std::string type;
      static bool              isOfType(const std::string& t) { return t == type; }
    };

    struct AprioriSmoothingType {
      static const std::string type;
      static bool              isOfType(const std::string& t) { return t == type; }
    };

    struct AprioriNoAprioriType {
      static const std::string type;
      static bool              isOfType(const std::string& t) { return t == type; }
    };

    struct AprioriBDeuType {
      static const std::string type;
      static bool              isOfType(const std::string& t) { return t == type; }
    };


  } /* namespace learning */

} /* namespace gum */

#endif /* GUM_LEARNING_A_PRIORI_TYPES_H */
