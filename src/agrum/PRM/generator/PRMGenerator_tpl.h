
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


/**
 * @file
 * @brief Inline implementation of PRMGenerator.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */
#include <agrum/PRM/generator/PRMGenerator.h>

namespace gum {
  namespace prm {

    template < typename GUM_SCALAR >
    INLINE PRMGenerator< GUM_SCALAR >::PRMGenerator() {
      GUM_CONSTRUCTOR(PRMGenerator);
    }

    template < typename GUM_SCALAR >
    INLINE PRMGenerator< GUM_SCALAR >::PRMGenerator(const PRMGenerator& source) :
        _name_gen(source._name_gen) {
      GUM_CONS_CPY(PRMGenerator);
    }

    template < typename GUM_SCALAR >
    INLINE PRMGenerator< GUM_SCALAR >::~PRMGenerator() {
      GUM_DESTRUCTOR(PRMGenerator);
    }

    template < typename GUM_SCALAR >
    INLINE void PRMGenerator< GUM_SCALAR >::setNameGenerator(
       const NameGenerator& name_gen) {
      _name_gen = name_gen;
    }

    template < typename GUM_SCALAR >
    INLINE const NameGenerator&
                 PRMGenerator< GUM_SCALAR >::getNameGenerator() const {
      return _name_gen;
    }

  } /* namespace prm */
} /* namespace gum */
