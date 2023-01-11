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
 * @brief Inline implementation of gum::PRMClassElement
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <agrum/PRM/elements/PRMClassElement.h>
#include <agrum/PRM/elements/PRMType.h>

namespace gum {
  namespace prm {

    template < typename GUM_SCALAR >
    PRMClassElement< GUM_SCALAR >::PRMClassElement(const std::string& name) : PRMObject(name) {
      GUM_CONSTRUCTOR(PRMClassElement);
    }

    template < typename GUM_SCALAR >
    PRMClassElement< GUM_SCALAR >::PRMClassElement(const PRMClassElement< GUM_SCALAR >& source) :
        PRMObject(source.name()), _id_(source.id()) {
      GUM_CONS_CPY(PRMClassElement);
    }

    template < typename GUM_SCALAR >
    PRMClassElement< GUM_SCALAR >::~PRMClassElement() {
      GUM_DESTRUCTOR(PRMClassElement);
    }

    template < typename GUM_SCALAR >
    INLINE NodeId PRMClassElement< GUM_SCALAR >::id() const {
      return _id_;
    }

    template < typename GUM_SCALAR >
    INLINE PRMObject::prm_type PRMClassElement< GUM_SCALAR >::obj_type() const {
      return prm_type::CLASS_ELT;
    }

    template < typename GUM_SCALAR >
    INLINE void PRMClassElement< GUM_SCALAR >::setId(NodeId id) {
      _id_ = id;
    }

    template < typename GUM_SCALAR >
    INLINE const std::string& PRMClassElement< GUM_SCALAR >::safeName() const {
      return safeName_;
    }

    template < typename GUM_SCALAR >
    INLINE std::string PRMClassElement< GUM_SCALAR >::cast(const PRMType& t) const {
      if (type().isSubTypeOf(t)) {
        return PRMObject::LEFT_CAST() + t.name() + PRMObject::RIGHT_CAST() + name();
      } else {
        GUM_ERROR(OperationNotAllowed, "illegal cast")
      }
    }

  } /* namespace prm */
} /* namespace gum */
