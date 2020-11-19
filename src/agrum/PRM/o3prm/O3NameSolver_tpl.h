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
 * @brief Implentation for the O3NameSolver class.
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 * @author Lionel TORTI
 */

#include <agrum/PRM/o3prm/O3NameSolver.h>

namespace gum {
  namespace prm {
    namespace o3prm {

      template < typename GUM_SCALAR >
      INLINE O3NameSolver< GUM_SCALAR >::O3NameSolver(PRM< GUM_SCALAR >& prm,
                                                      O3PRM&             o3_prm,
                                                      ErrorsContainer&   errors) :
          prm__(&prm),
          o3_prm__(&o3_prm), errors__(&errors) {
        GUM_CONSTRUCTOR(O3NameSolver);
      }

      template < typename GUM_SCALAR >
      INLINE O3NameSolver< GUM_SCALAR >::O3NameSolver(
         const O3NameSolver< GUM_SCALAR >& src) :
          prm__(src.prm__),
          o3_prm__(src.o3_prm__), errors__(src.errors__),
          typeName__(src.typeName__), eltName__(src.eltName__),
          refName__(src.refName__), interfaceName__(src.interfaceName__),
          className__(src.className__) {
        GUM_CONS_CPY(O3NameSolver);
      }

      template < typename GUM_SCALAR >
      INLINE O3NameSolver< GUM_SCALAR >::O3NameSolver(
         O3NameSolver< GUM_SCALAR >&& src) :
          prm__(std::move(src.prm__)),
          o3_prm__(std::move(src.o3_prm__)), errors__(std::move(src.errors__)),
          typeName__(std::move(src.typeName__)),
          eltName__(std::move(src.eltName__)), refName__(std::move(src.refName__)),
          interfaceName__(std::move(src.interfaceName__)),
          className__(std::move(src.className__)) {
        GUM_CONS_MOV(O3NameSolver);
      }

      template < typename GUM_SCALAR >
      INLINE O3NameSolver< GUM_SCALAR >::~O3NameSolver() {
        GUM_DESTRUCTOR(O3NameSolver);
      }

      template < typename GUM_SCALAR >
      INLINE O3NameSolver< GUM_SCALAR >& O3NameSolver< GUM_SCALAR >::operator=(
         const O3NameSolver< GUM_SCALAR >& src) {
        if (this == &src) { return *this; }
        prm__ = src.prm__;
        o3_prm__ = src.o3_prm__;
        errors__ = src.errors__;
        typeName__ = src.typeName__;
        eltName__ = src.eltName__;
        refName__ = src.refName__;
        interfaceName__ = src.interfaceName__;
        className__ = src.className__;
        return *this;
      }

      template < typename GUM_SCALAR >
      INLINE O3NameSolver< GUM_SCALAR >&
         O3NameSolver< GUM_SCALAR >::operator=(O3NameSolver< GUM_SCALAR >&& src) {
        if (this == &src) { return *this; }
        prm__ = std::move(src.prm__);
        o3_prm__ = std::move(src.o3_prm__);
        errors__ = std::move(src.errors__);
        typeName__ = std::move(src.typeName__);
        eltName__ = std::move(src.eltName__);
        refName__ = std::move(src.refName__);
        interfaceName__ = std::move(src.interfaceName__);
        className__ = std::move(src.className__);
        return *this;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3NameSolver< GUM_SCALAR >::resolveClassElement(O3Label& name) {
        // If empty string, we return an empty string
        if (name.label() == "") { return true; }
        // If we've already found the element real name
        if (eltName__.exists(name.label())) {
          name.label() = eltName__[name.label()];
          return true;
        }
        // If name exists as is
        if (prm__->isType(name.label())) {
          eltName__.insert(name.label(), name.label());
          return true;
        }
        // If name exists as is
        if (prm__->isInterface(name.label())) {
          eltName__.insert(name.label(), name.label());
          return true;
        }
        // If name exists as is
        if (prm__->isClass(name.label())) {
          eltName__.insert(name.label(), name.label());
          return true;
        }
        // If name exists as is in O3PRM types
        for (auto& t: o3_prm__->types()) {
          if (t->name().label() == name.label()) {
            eltName__.insert(name.label(), name.label());
            return true;
          }
        }
        // If name exists as is in O3PRM interfaces
        for (auto& i: o3_prm__->interfaces()) {
          if (i->name().label() == name.label()) {
            eltName__.insert(name.label(), name.label());
            return true;
          }
        }
        // If name exists as is in O3PRM classes
        for (auto& c: o3_prm__->classes()) {
          if (c->name().label() == name.label()) {
            eltName__.insert(name.label(), name.label());
            return true;
          }
        }

        auto lookup = "." + name.label();
        auto found = Set< std::string >();
        auto matches = std::vector< std::string >();

        // Trying with types
        for (auto t: prm__->types()) {
          if (endsWith(t->name(), lookup)) {
            if (!found.exists(t->name())) {
              found.insert(t->name());
              matches.push_back(t->name());
            }
          }
        }
        // Trying with O3Types
        for (auto& t: o3_prm__->types()) {
          if (endsWith(t->name().label(), lookup)) {
            if (!found.exists(t->name().label())) {
              found.insert(t->name().label());
              matches.push_back(t->name().label());
            }
          }
        }

        // Trying with interfaces
        for (auto i: prm__->interfaces()) {
          if (endsWith(i->name(), lookup)) {
            if (!found.exists(i->name())) {
              found.insert(i->name());
              matches.push_back(i->name());
            }
          }
        }
        // Trying with O3Interface
        for (auto& i: o3_prm__->interfaces()) {
          if (endsWith(i->name().label(), lookup)) {
            if (!found.exists(i->name().label())) {
              found.insert(i->name().label());
              matches.push_back(i->name().label());
            }
          }
        }

        // Trying with class
        for (auto c: prm__->classes()) {
          if (endsWith(c->name(), lookup)) {
            if (!found.exists(c->name())) {
              found.insert(c->name());
              matches.push_back(c->name());
            }
          }
        }
        // Trying with O3Class
        for (auto& c: o3_prm__->classes()) {
          if (endsWith(c->name().label(), lookup)) {
            if (!found.exists(c->name().label())) {
              found.insert(c->name().label());
              matches.push_back(c->name().label());
            }
          }
        }

        if (matches.size() == 1) {   // One match is good
          eltName__.insert(name.label(), matches.back());
          name.label() = matches.back();
          return true;

        } else if (matches.size() == 0) {   // 0 match is not found

          // Unknown name type
          O3PRM_TYPE_NOT_FOUND(name, *errors__);
          return false;

        } else {   // More than one match is ambiguous

          // Ambiguous name
          O3PRM_TYPE_AMBIGUOUS(name, matches, *errors__);
          return false;
        }
      }

      template < typename GUM_SCALAR >
      INLINE bool O3NameSolver< GUM_SCALAR >::resolveType(O3Label& name) {
        // If empty string, we return an empty string
        if (name.label() == "") { return true; }

        // If we've already found the type real name
        if (typeName__.exists(name.label())) {
          name.label() = typeName__[name.label()];
          return true;
        }

        // If name exists as is in PRM
        if (prm__->isType(name.label())) {
          typeName__.insert(name.label(), name.label());
          return true;
        }

        // If name exists as is in O3PRM
        for (auto& t: o3_prm__->types()) {
          if (t->name().label() == name.label()) {
            typeName__.insert(name.label(), name.label());
            return true;
          }
        }

        // If we didn't find it as is, then we must find a namespace
        // in which it was declared
        auto lookup = "." + name.label();
        auto found = Set< std::string >();
        auto matches = std::vector< std::string >();

        // Trying with types
        for (auto t: prm__->types()) {
          if (endsWith(t->name(), lookup)) {
            if (!found.exists(t->name())) {
              found.insert(t->name());
              matches.push_back(t->name());
            }
          }
        }

        // Trying with O3Types
        for (auto& t: o3_prm__->types()) {
          if (endsWith(t->name().label(), lookup)) {
            if (!found.exists(t->name().label())) {
              found.insert(t->name().label());
              matches.push_back(t->name().label());
            }
          }
        }

        if (matches.size() == 1) {   // One match is good
          typeName__.insert(name.label(), matches.back());
          name.label() = matches.back();
          return true;

        } else if (matches.size() == 0) {   // 0 match is not found

          // Unknown name type
          O3PRM_TYPE_NOT_FOUND(name, *errors__);
          return false;

        } else {   // More than one match is ambiguous

          // Ambiguous name
          O3PRM_TYPE_AMBIGUOUS(name, matches, *errors__);
          return false;
        }
      }

      template < typename GUM_SCALAR >
      INLINE bool O3NameSolver< GUM_SCALAR >::resolveInterface(O3Label& name) {
        // If empty string, we return an empty string
        if (name.label() == "") { return true; }

        // If we've already found the interface real name
        if (interfaceName__.exists(name.label())) {
          name.label() = interfaceName__[name.label()];
          return true;
        }

        // If name exists as is
        if (prm__->isInterface(name.label())) {
          interfaceName__.insert(name.label(), name.label());
          return true;
        }

        for (auto& i: o3_prm__->interfaces()) {
          if (i->name().label() == name.label()) {
            interfaceName__.insert(name.label(), name.label());
            return true;
          }
        }

        // If we didn't find it as is, then we must find a namespace
        // in which it was declared
        auto lookup = "." + name.label();
        auto found = Set< std::string >();
        auto matches = std::vector< std::string >();

        // Trying with interfaces
        for (auto i: prm__->interfaces()) {
          if (endsWith(i->name(), lookup)) {
            if (!found.exists(i->name())) {
              found.insert(i->name());
              matches.push_back(i->name());
            }
          }
        }

        // Trying with O3Interface
        for (auto& i: o3_prm__->interfaces()) {
          if (endsWith(i->name().label(), lookup)) {
            if (!found.exists(i->name().label())) {
              found.insert(i->name().label());
              matches.push_back(i->name().label());
            }
          }
        }

        if (matches.size() == 1) {   // One match is good

          interfaceName__.insert(name.label(), matches.back());
          name.label() = matches.back();
          return true;

        } else if (matches.size() == 0) {   // 0 match is not found

          // Unknown name type
          O3PRM_INTERFACE_NOT_FOUND(name, *errors__);
          return false;

        } else {   // More than one match is ambiguous

          // Ambiguous name
          O3PRM_INTERFACE_AMBIGUOUS(name, matches, *errors__);
          return false;
        }
      }

      template < typename GUM_SCALAR >
      INLINE bool O3NameSolver< GUM_SCALAR >::resolveClass(O3Label& name) {
        // If empty string, we return an empty string
        if (name.label() == "") { return true; }

        // If we've already found super real name
        if (className__.exists(name.label())) {
          name.label() = className__[name.label()];
          return true;
        }

        // If class name exists as is
        if (prm__->isClass(name.label())) {
          className__.insert(name.label(), name.label());
          return true;
        }

        for (auto& c: o3_prm__->classes()) {
          if (c->name().label() == name.label()) {
            className__.insert(name.label(), name.label());
            return true;
          }
        }

        // If we didn't find it as is, then we must find a namespace
        // in which it was declared
        auto lookup = "." + name.label();
        auto matches = std::vector< std::string >();
        auto found = Set< std::string >();

        // Try to complete with Class
        for (auto c: prm__->classes()) {
          if (endsWith(c->name(), lookup)) {
            if (!found.exists(c->name())) {
              found.insert(c->name());
              matches.push_back(c->name());
            }
          }
        }

        // Try to complete with O3Class
        for (auto& c: o3_prm__->classes()) {
          if (endsWith(c->name().label(), lookup)) {
            if (!found.exists(c->name().label())) {
              found.insert(c->name().label());
              matches.push_back(c->name().label());
            }
          }
        }

        if (matches.size() == 1) {   // One match is good

          className__.insert(name.label(), matches.back());
          name.label() = matches.back();
          return true;

        } else if (matches.size() == 0) {   // 0 match is not found

          // Unknown super class
          O3PRM_CLASS_NOT_FOUND(name, *errors__);
          return false;

        } else {   // More than one match is ambiguous

          // Ambiguous name
          O3PRM_CLASS_AMBIGUOUS(name, matches, *errors__);
          return false;
        }
      }

      template < typename GUM_SCALAR >
      INLINE bool O3NameSolver< GUM_SCALAR >::resolveSlotType(O3Label& name) {
        // If empty string, we return an empty string
        if (name.label() == "") { return true; }
        // If we've already found the reference real name
        if (refName__.exists(name.label())) {
          name.label() = refName__[name.label()];
          return true;
        }
        // If name exists as is
        if (prm__->isInterface(name.label()) || prm__->isClass(name.label())) {
          refName__.insert(name.label(), name.label());
          return true;
        }

        // We check if it matches an O3Interface
        for (auto& i: o3_prm__->interfaces()) {
          if (i->name().label() == name.label()) {
            interfaceName__.insert(name.label(), name.label());
            return true;
          }
        }

        // We check if it matches an O3Class
        for (auto& c: o3_prm__->classes()) {
          if (c->name().label() == name.label()) {
            className__.insert(name.label(), name.label());
            return true;
          }
        }

        // If we didn't find it as is, then we must find a namespace
        // in which it was declared
        auto lookup = "." + name.label();
        auto found = Set< std::string >();
        auto matches = std::vector< std::string >();

        // Trying with interfaces
        for (auto i: prm__->interfaces()) {
          if (endsWith(i->name(), lookup)) {
            if (!found.exists(i->name())) {
              found.insert(i->name());
              matches.push_back(i->name());
            }
          }
        }

        // Trying with O3Interface
        for (auto& i: o3_prm__->interfaces()) {
          if (endsWith(i->name().label(), lookup)) {
            if (!found.exists(i->name().label())) {
              found.insert(i->name().label());
              matches.push_back(i->name().label());
            }
          }
        }

        // Try to complete with Class
        for (auto c: prm__->classes()) {
          if (endsWith(c->name(), lookup)) {
            if (!found.exists(c->name())) {
              found.insert(c->name());
              matches.push_back(c->name());
            }
          }
        }

        // Try to complete with O3Class
        for (auto& c: o3_prm__->classes()) {
          if (endsWith(c->name().label(), lookup)) {
            if (!found.exists(c->name().label())) {
              found.insert(c->name().label());
              matches.push_back(c->name().label());
            }
          }
        }

        if (matches.size() == 1) {   // One match is good

          refName__.insert(name.label(), matches.back());
          name.label() = matches.back();
          return true;

        } else if (matches.size() == 0) {   // 0 match is not found

          // Unknown name type
          O3PRM_REFERENCE_NOT_FOUND(name, *errors__);
          return false;

        } else {   // More than one match is ambiguous

          // Ambiguous name
          O3PRM_REFERENCE_AMBIGUOUS(name, matches, *errors__);
          return false;
        }
      }


    }   // namespace o3prm
  }     // namespace prm
}   // namespace gum
