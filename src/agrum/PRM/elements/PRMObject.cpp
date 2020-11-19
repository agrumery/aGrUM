/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief Source implementation of PRMObject.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(@LIP6)
 *
 */

#include <agrum/PRM/elements/PRMObject.h>

#ifdef GUM_NO_INLINE
#  include <agrum/PRM/elements/PRMObject_inl.h>
#endif   // GUM_NO_INLINE

namespace gum {
  namespace prm {
    // ==========================================================================
    // Constructor.
    // @param type The type of this object.
    // ==========================================================================
    PRMObject::PRMObject(const std::string& name) : name__(name) {
      GUM_CONSTRUCTOR(PRMObject);
    }
    // ==========================================================================
    // Copy constructor.
    // ==========================================================================
    PRMObject::PRMObject(const PRMObject& source) : name__(source.name__) {
      GUM_CONS_CPY(PRMObject);
    }
    // ==========================================================================
    // Move constructor.
    // ==========================================================================
    PRMObject::PRMObject(PRMObject&& source) : name__(std::move(source.name__)) {
      GUM_CONS_CPY(PRMObject);
    }
    // ==========================================================================
    // Destructor.
    // ==========================================================================
    PRMObject::~PRMObject() { GUM_DESTRUCTOR(PRMObject); }

    // Copy operator.
    PRMObject& PRMObject::operator=(const PRMObject& source) {
      name__ = source.name__;
      return *this;
    }

    // Move operator.
    PRMObject& PRMObject::operator=(PRMObject&& source) {
      name__ = std::move(source.name__);
      return *this;
    }

    // ==========================================================================
    // For printing PRMType easily.
    // ==========================================================================
    std::ostream& operator<<(std::ostream& out, PRMObject::prm_type obj_type) {
      return (out << PRMObject::enum2str(obj_type));
    }
    // ==========================================================================
  } /* namespace prm */
} /* namespace gum */
