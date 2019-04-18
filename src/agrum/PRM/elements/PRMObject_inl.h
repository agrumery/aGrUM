
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
 * @brief Inline implementation of PRMObject.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

namespace gum {
  namespace prm {

    // Returns the name of this object.
    INLINE
    const std::string& PRMObject::name() const { return __name; }

    // Returns the name of this object.
    INLINE
    void PRMObject::name(const std::string& name) { __name = name; }

    // To PRMObject are equal if they have the same name (which is unique).
    INLINE
    bool PRMObject::operator==(const PRMObject& obj) const {
      return __name == obj.name();
    }

    // To PRMObject are equal if they have the same name (which is unique).
    INLINE
    bool PRMObject::operator!=(const PRMObject& obj) const {
      return __name != obj.name();
    }

  } /* namespace prm */
} /* namespace gum */
