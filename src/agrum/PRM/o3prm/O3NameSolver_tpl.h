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
 * @brief Implentation for the O3NameSolver class.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
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
          _prm_(&prm),
          _o3_prm_(&o3_prm), _errors_(&errors) {
        GUM_CONSTRUCTOR(O3NameSolver);
      }

      template < typename GUM_SCALAR >
      INLINE O3NameSolver< GUM_SCALAR >::O3NameSolver(const O3NameSolver< GUM_SCALAR >& src) :
          _prm_(src._prm_), _o3_prm_(src._o3_prm_), _errors_(src._errors_),
          _typeName_(src._typeName_), _eltName_(src._eltName_), _refName_(src._refName_),
          _interfaceName_(src._interfaceName_), _className_(src._className_) {
        GUM_CONS_CPY(O3NameSolver);
      }

      template < typename GUM_SCALAR >
      INLINE O3NameSolver< GUM_SCALAR >::O3NameSolver(O3NameSolver< GUM_SCALAR >&& src) :
          _prm_(std::move(src._prm_)), _o3_prm_(std::move(src._o3_prm_)),
          _errors_(std::move(src._errors_)), _typeName_(std::move(src._typeName_)),
          _eltName_(std::move(src._eltName_)), _refName_(std::move(src._refName_)),
          _interfaceName_(std::move(src._interfaceName_)), _className_(std::move(src._className_)) {
        GUM_CONS_MOV(O3NameSolver);
      }

      template < typename GUM_SCALAR >
      INLINE O3NameSolver< GUM_SCALAR >::~O3NameSolver() {
        GUM_DESTRUCTOR(O3NameSolver);
      }

      template < typename GUM_SCALAR >
      INLINE O3NameSolver< GUM_SCALAR >&
         O3NameSolver< GUM_SCALAR >::operator=(const O3NameSolver< GUM_SCALAR >& src) {
        if (this == &src) { return *this; }
        _prm_           = src._prm_;
        _o3_prm_        = src._o3_prm_;
        _errors_        = src._errors_;
        _typeName_      = src._typeName_;
        _eltName_       = src._eltName_;
        _refName_       = src._refName_;
        _interfaceName_ = src._interfaceName_;
        _className_     = src._className_;
        return *this;
      }

      template < typename GUM_SCALAR >
      INLINE O3NameSolver< GUM_SCALAR >&
         O3NameSolver< GUM_SCALAR >::operator=(O3NameSolver< GUM_SCALAR >&& src) {
        if (this == &src) { return *this; }
        _prm_           = std::move(src._prm_);
        _o3_prm_        = std::move(src._o3_prm_);
        _errors_        = std::move(src._errors_);
        _typeName_      = std::move(src._typeName_);
        _eltName_       = std::move(src._eltName_);
        _refName_       = std::move(src._refName_);
        _interfaceName_ = std::move(src._interfaceName_);
        _className_     = std::move(src._className_);
        return *this;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3NameSolver< GUM_SCALAR >::resolveClassElement(O3Label& name) {
        // If empty string, we return an empty string
        if (name.label().empty()) { return true; }
        // If we've already found the element real name
        if (_eltName_.exists(name.label())) {
          name.label() = _eltName_[name.label()];
          return true;
        }
        // If name exists as is
        if (_prm_->isType(name.label())) {
          _eltName_.insert(name.label(), name.label());
          return true;
        }
        // If name exists as is
        if (_prm_->isInterface(name.label())) {
          _eltName_.insert(name.label(), name.label());
          return true;
        }
        // If name exists as is
        if (_prm_->isClass(name.label())) {
          _eltName_.insert(name.label(), name.label());
          return true;
        }
        // If name exists as is in O3PRM types
        for (auto& t: _o3_prm_->types()) {
          if (t->name().label() == name.label()) {
            _eltName_.insert(name.label(), name.label());
            return true;
          }
        }
        // If name exists as is in O3PRM interfaces
        for (auto& i: _o3_prm_->interfaces()) {
          if (i->name().label() == name.label()) {
            _eltName_.insert(name.label(), name.label());
            return true;
          }
        }
        // If name exists as is in O3PRM classes
        for (auto& c: _o3_prm_->classes()) {
          if (c->name().label() == name.label()) {
            _eltName_.insert(name.label(), name.label());
            return true;
          }
        }

        auto lookup  = "." + name.label();
        auto found   = Set< std::string >();
        auto matches = std::vector< std::string >();

        // Trying with types
        for (auto t: _prm_->types()) {
          if (endsWith(t->name(), lookup)) {
            if (!found.exists(t->name())) {
              found.insert(t->name());
              matches.push_back(t->name());
            }
          }
        }
        // Trying with O3Types
        for (auto& t: _o3_prm_->types()) {
          if (endsWith(t->name().label(), lookup)) {
            if (!found.exists(t->name().label())) {
              found.insert(t->name().label());
              matches.push_back(t->name().label());
            }
          }
        }

        // Trying with interfaces
        for (auto i: _prm_->interfaces()) {
          if (endsWith(i->name(), lookup)) {
            if (!found.exists(i->name())) {
              found.insert(i->name());
              matches.push_back(i->name());
            }
          }
        }
        // Trying with O3Interface
        for (auto& i: _o3_prm_->interfaces()) {
          if (endsWith(i->name().label(), lookup)) {
            if (!found.exists(i->name().label())) {
              found.insert(i->name().label());
              matches.push_back(i->name().label());
            }
          }
        }

        // Trying with class
        for (auto c: _prm_->classes()) {
          if (endsWith(c->name(), lookup)) {
            if (!found.exists(c->name())) {
              found.insert(c->name());
              matches.push_back(c->name());
            }
          }
        }
        // Trying with O3Class
        for (auto& c: _o3_prm_->classes()) {
          if (endsWith(c->name().label(), lookup)) {
            if (!found.exists(c->name().label())) {
              found.insert(c->name().label());
              matches.push_back(c->name().label());
            }
          }
        }

        if (matches.size() == 1) {   // One match is good
          _eltName_.insert(name.label(), matches.back());
          name.label() = matches.back();
          return true;

        } else if (matches.size() == 0) {   // 0 match is not found

          // Unknown name type
          O3PRM_TYPE_NOT_FOUND(name, *_errors_);
          return false;

        } else {   // More than one match is ambiguous

          // Ambiguous name
          O3PRM_TYPE_AMBIGUOUS(name, matches, *_errors_);
          return false;
        }
      }

      template < typename GUM_SCALAR >
      INLINE bool O3NameSolver< GUM_SCALAR >::resolveType(O3Label& name) {
        // If empty string, we return an empty string
        if (name.label().empty()) { return true; }

        // If we've already found the type real name
        if (_typeName_.exists(name.label())) {
          name.label() = _typeName_[name.label()];
          return true;
        }

        // If name exists as is in PRM
        if (_prm_->isType(name.label())) {
          _typeName_.insert(name.label(), name.label());
          return true;
        }

        // If name exists as is in O3PRM
        for (auto& t: _o3_prm_->types()) {
          if (t->name().label() == name.label()) {
            _typeName_.insert(name.label(), name.label());
            return true;
          }
        }

        // If we didn't find it as is, then we must find a namespace
        // in which it was declared
        auto lookup  = "." + name.label();
        auto found   = Set< std::string >();
        auto matches = std::vector< std::string >();

        // Trying with types
        for (auto t: _prm_->types()) {
          if (endsWith(t->name(), lookup)) {
            if (!found.exists(t->name())) {
              found.insert(t->name());
              matches.push_back(t->name());
            }
          }
        }

        // Trying with O3Types
        for (auto& t: _o3_prm_->types()) {
          if (endsWith(t->name().label(), lookup)) {
            if (!found.exists(t->name().label())) {
              found.insert(t->name().label());
              matches.push_back(t->name().label());
            }
          }
        }

        if (matches.size() == 1) {   // One match is good
          _typeName_.insert(name.label(), matches.back());
          name.label() = matches.back();
          return true;

        } else if (matches.size() == 0) {   // 0 match is not found

          // Unknown name type
          O3PRM_TYPE_NOT_FOUND(name, *_errors_);
          return false;

        } else {   // More than one match is ambiguous

          // Ambiguous name
          O3PRM_TYPE_AMBIGUOUS(name, matches, *_errors_);
          return false;
        }
      }

      template < typename GUM_SCALAR >
      INLINE bool O3NameSolver< GUM_SCALAR >::resolveInterface(O3Label& name) {
        // If empty string, we return an empty string
        if (name.label().empty()) { return true; }

        // If we've already found the interface real name
        if (_interfaceName_.exists(name.label())) {
          name.label() = _interfaceName_[name.label()];
          return true;
        }

        // If name exists as is
        if (_prm_->isInterface(name.label())) {
          _interfaceName_.insert(name.label(), name.label());
          return true;
        }

        for (auto& i: _o3_prm_->interfaces()) {
          if (i->name().label() == name.label()) {
            _interfaceName_.insert(name.label(), name.label());
            return true;
          }
        }

        // If we didn't find it as is, then we must find a namespace
        // in which it was declared
        auto lookup  = "." + name.label();
        auto found   = Set< std::string >();
        auto matches = std::vector< std::string >();

        // Trying with interfaces
        for (auto i: _prm_->interfaces()) {
          if (endsWith(i->name(), lookup)) {
            if (!found.exists(i->name())) {
              found.insert(i->name());
              matches.push_back(i->name());
            }
          }
        }

        // Trying with O3Interface
        for (auto& i: _o3_prm_->interfaces()) {
          if (endsWith(i->name().label(), lookup)) {
            if (!found.exists(i->name().label())) {
              found.insert(i->name().label());
              matches.push_back(i->name().label());
            }
          }
        }

        if (matches.size() == 1) {   // One match is good

          _interfaceName_.insert(name.label(), matches.back());
          name.label() = matches.back();
          return true;

        } else if (matches.size() == 0) {   // 0 match is not found

          // Unknown name type
          O3PRM_INTERFACE_NOT_FOUND(name, *_errors_);
          return false;

        } else {   // More than one match is ambiguous

          // Ambiguous name
          O3PRM_INTERFACE_AMBIGUOUS(name, matches, *_errors_);
          return false;
        }
      }

      template < typename GUM_SCALAR >
      INLINE bool O3NameSolver< GUM_SCALAR >::resolveClass(O3Label& name) {
        // If empty string, we return an empty string
        if (name.label().empty()) { return true; }

        // If we've already found super real name
        if (_className_.exists(name.label())) {
          name.label() = _className_[name.label()];
          return true;
        }

        // If class name exists as is
        if (_prm_->isClass(name.label())) {
          _className_.insert(name.label(), name.label());
          return true;
        }

        for (auto& c: _o3_prm_->classes()) {
          if (c->name().label() == name.label()) {
            _className_.insert(name.label(), name.label());
            return true;
          }
        }

        // If we didn't find it as is, then we must find a namespace
        // in which it was declared
        auto lookup  = "." + name.label();
        auto matches = std::vector< std::string >();
        auto found   = Set< std::string >();

        // Try to complete with Class
        for (auto c: _prm_->classes()) {
          if (endsWith(c->name(), lookup)) {
            if (!found.exists(c->name())) {
              found.insert(c->name());
              matches.push_back(c->name());
            }
          }
        }

        // Try to complete with O3Class
        for (auto& c: _o3_prm_->classes()) {
          if (endsWith(c->name().label(), lookup)) {
            if (!found.exists(c->name().label())) {
              found.insert(c->name().label());
              matches.push_back(c->name().label());
            }
          }
        }

        if (matches.size() == 1) {   // One match is good

          _className_.insert(name.label(), matches.back());
          name.label() = matches.back();
          return true;

        } else if (matches.size() == 0) {   // 0 match is not found

          // Unknown super class
          O3PRM_CLASS_NOT_FOUND(name, *_errors_);
          return false;

        } else {   // More than one match is ambiguous

          // Ambiguous name
          O3PRM_CLASS_AMBIGUOUS(name, matches, *_errors_);
          return false;
        }
      }

      template < typename GUM_SCALAR >
      INLINE bool O3NameSolver< GUM_SCALAR >::resolveSlotType(O3Label& name) {
        // If empty string, we return an empty string
        if (name.label().empty()) { return true; }
        // If we've already found the reference real name
        if (_refName_.exists(name.label())) {
          name.label() = _refName_[name.label()];
          return true;
        }
        // If name exists as is
        if (_prm_->isInterface(name.label()) || _prm_->isClass(name.label())) {
          _refName_.insert(name.label(), name.label());
          return true;
        }

        // We check if it matches an O3Interface
        for (auto& i: _o3_prm_->interfaces()) {
          if (i->name().label() == name.label()) {
            _interfaceName_.insert(name.label(), name.label());
            return true;
          }
        }

        // We check if it matches an O3Class
        for (auto& c: _o3_prm_->classes()) {
          if (c->name().label() == name.label()) {
            _className_.insert(name.label(), name.label());
            return true;
          }
        }

        // If we didn't find it as is, then we must find a namespace
        // in which it was declared
        auto lookup  = "." + name.label();
        auto found   = Set< std::string >();
        auto matches = std::vector< std::string >();

        // Trying with interfaces
        for (auto i: _prm_->interfaces()) {
          if (endsWith(i->name(), lookup)) {
            if (!found.exists(i->name())) {
              found.insert(i->name());
              matches.push_back(i->name());
            }
          }
        }

        // Trying with O3Interface
        for (auto& i: _o3_prm_->interfaces()) {
          if (endsWith(i->name().label(), lookup)) {
            if (!found.exists(i->name().label())) {
              found.insert(i->name().label());
              matches.push_back(i->name().label());
            }
          }
        }

        // Try to complete with Class
        for (auto c: _prm_->classes()) {
          if (endsWith(c->name(), lookup)) {
            if (!found.exists(c->name())) {
              found.insert(c->name());
              matches.push_back(c->name());
            }
          }
        }

        // Try to complete with O3Class
        for (auto& c: _o3_prm_->classes()) {
          if (endsWith(c->name().label(), lookup)) {
            if (!found.exists(c->name().label())) {
              found.insert(c->name().label());
              matches.push_back(c->name().label());
            }
          }
        }

        if (matches.size() == 1) {   // One match is good

          _refName_.insert(name.label(), matches.back());
          name.label() = matches.back();
          return true;

        } else if (matches.size() == 0) {   // 0 match is not found

          // Unknown name type
          O3PRM_REFERENCE_NOT_FOUND(name, *_errors_);
          return false;

        } else {   // More than one match is ambiguous

          // Ambiguous name
          O3PRM_REFERENCE_AMBIGUOUS(name, matches, *_errors_);
          return false;
        }
      }


    }   // namespace o3prm
  }     // namespace prm
}   // namespace gum
