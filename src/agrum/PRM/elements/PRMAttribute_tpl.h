/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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


/**
 * @file
 * @brief Inline implementation of gum::PRMAttribute
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

namespace gum {
  namespace prm {

    template < typename GUM_SCALAR >
    PRMAttribute< GUM_SCALAR >::PRMAttribute(const std::string& name) :
        PRMClassElement< GUM_SCALAR >(name) {
      GUM_CONSTRUCTOR(PRMAttribute);
    }

    template < typename GUM_SCALAR >
    PRMAttribute< GUM_SCALAR >::PRMAttribute(const PRMAttribute< GUM_SCALAR >& source) :
        PRMClassElement< GUM_SCALAR >(source) {
      GUM_CONS_CPY(PRMAttribute);
      GUM_ERROR(OperationNotAllowed, "Cannot copy Attributes")
    }

    template < typename GUM_SCALAR >
    PRMAttribute< GUM_SCALAR >::~PRMAttribute() {
      GUM_DESTRUCTOR(PRMAttribute);
    }

    template < typename GUM_SCALAR >
    void PRMAttribute< GUM_SCALAR >::overload(PRMAttribute< GUM_SCALAR >* source) {
      auto old_type = this->type_();
      this->type_(source->type_());
      source->type_(old_type);
    }

  } /* namespace prm */
} /* namespace gum */
