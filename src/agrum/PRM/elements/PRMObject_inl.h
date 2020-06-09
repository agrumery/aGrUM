
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


/**
 * @file
 * @brief Inline implementation of PRMObject.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(@LIP6)
 */

namespace gum {
  namespace prm {

    // Returns the name of this object.
    INLINE
    const std::string& PRMObject::name() const { return name__; }

    // Returns the name of this object.
    INLINE
    void PRMObject::name(const std::string& name) { name__ = name; }

    // To PRMObject are equal if they have the same name (which is unique).
    INLINE
    bool PRMObject::operator==(const PRMObject& obj) const {
      return name__ == obj.name();
    }

    // To PRMObject are equal if they have the same name (which is unique).
    INLINE
    bool PRMObject::operator!=(const PRMObject& obj) const {
      return name__ != obj.name();
    }

  } /* namespace prm */
} /* namespace gum */
