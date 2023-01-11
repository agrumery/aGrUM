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
 * @brief Implementation for the O3TypeFactory class.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 * @author Lionel TORTI
 */

#include <agrum/PRM/o3prm/O3TypeFactory.h>

namespace gum {
  namespace prm {
    namespace o3prm {

      template < typename GUM_SCALAR >
      INLINE O3TypeFactory< GUM_SCALAR >::O3TypeFactory(PRM< GUM_SCALAR >&          prm,
                                                        O3PRM&                      o3_prm,
                                                        O3NameSolver< GUM_SCALAR >& solver,
                                                        ErrorsContainer&            errors) :
          _prm_(&prm),
          _o3_prm_(&o3_prm), _solver_(&solver), _errors_(&errors), _build_(false) {
        GUM_CONSTRUCTOR(O3TypeFactory);
      }

      template < typename GUM_SCALAR >
      INLINE O3TypeFactory< GUM_SCALAR >::O3TypeFactory(const O3TypeFactory< GUM_SCALAR >& src) :
          _prm_(src._prm_), _o3_prm_(src._o3_prm_), _solver_(src._solver_), _errors_(src._errors_),
          _build_(src._build_) {
        GUM_CONS_CPY(O3TypeFactory);
      }

      template < typename GUM_SCALAR >
      INLINE O3TypeFactory< GUM_SCALAR >::O3TypeFactory(O3TypeFactory< GUM_SCALAR >&& src) :
          _prm_(std::move(src._prm_)), _o3_prm_(std::move(src._o3_prm_)),
          _solver_(std::move(src._solver_)), _errors_(std::move(src._errors_)),
          _build_(std::move(src._build_)) {
        GUM_CONS_MOV(O3TypeFactory);
      }

      template < typename GUM_SCALAR >
      INLINE O3TypeFactory< GUM_SCALAR >::~O3TypeFactory() {
        GUM_DESTRUCTOR(O3TypeFactory);
      }

      template < typename GUM_SCALAR >
      INLINE O3TypeFactory< GUM_SCALAR >&
         O3TypeFactory< GUM_SCALAR >::operator=(const O3TypeFactory< GUM_SCALAR >& src) {
        if (this == &src) { return *this; }
        _prm_    = src._prm_;
        _o3_prm_ = src._o3_prm_;
        _solver_ = src._solver_;
        _errors_ = src._errors_;
        _build_  = src._build_;
        return *this;
      }

      template < typename GUM_SCALAR >
      INLINE O3TypeFactory< GUM_SCALAR >&
         O3TypeFactory< GUM_SCALAR >::operator=(O3TypeFactory< GUM_SCALAR >&& src) {
        if (this == &src) { return *this; }
        _prm_    = std::move(src._prm_);
        _o3_prm_ = std::move(src._o3_prm_);
        _solver_ = std::move(src._solver_);
        _errors_ = std::move(src._errors_);
        _build_  = std::move(src._build_);
        return *this;
      }

      template < typename GUM_SCALAR >
      INLINE void O3TypeFactory< GUM_SCALAR >::build() {
        if (_build_) {
          GUM_ERROR(FatalError, "types have already been built, change PRM to rebuild")
        }
        _build_ = true;

        // checking deprecated declarations
        _checkDepreactedO3Types_();

        // building types
        _buildTypes_();

        // building int types
        _buildIntTypes_();

        // building real types
        _buildRealTypes_();
      }

      template < typename GUM_SCALAR >
      INLINE void O3TypeFactory< GUM_SCALAR >::_buildTypes_() {
        if (_checkO3Types_()) {
          _setO3TypeCreationOrder_();

          PRMFactory< GUM_SCALAR > factory(_prm_);
          for (auto type: _o3Types_) {
            if (!_isPrimitiveType_(*type)) {
              if (_solver_->resolveType(type->superLabel())) {
                factory.startDiscreteType(type->name().label(), type->superLabel().label());

                for (auto& label: type->labels()) {
                  factory.addLabel(label.first.label(), label.second.label());
                }

                factory.endDiscreteType();
              }
            }
          }
        }
      }

      template < typename GUM_SCALAR >
      INLINE void O3TypeFactory< GUM_SCALAR >::_checkDepreactedO3Types_() {
        for (auto& t: _o3_prm_->types()) {
          if (t->deprecated()) { O3PRM_DEPRECATED_TYPE_WARNING(t->name(), *_errors_); }
        }

        for (auto& t: _o3_prm_->int_types()) {
          if (t->deprecated()) { O3PRM_DEPRECATED_TYPE_WARNING(t->name(), *_errors_); }
        }

        for (auto& t: _o3_prm_->real_types()) {
          if (t->deprecated()) { O3PRM_DEPRECATED_TYPE_WARNING(t->name(), *_errors_); }
        }
      }

      template < typename GUM_SCALAR >
      INLINE void O3TypeFactory< GUM_SCALAR >::_buildIntTypes_() {
        if (_checkO3IntTypes_()) {
          PRMFactory< GUM_SCALAR > factory(_prm_);
          for (auto type: _o3IntTypes_) {
            factory.addRangeType(type->name().label(), type->start().value(), type->end().value());
          }
        }
      }

      template < typename GUM_SCALAR >
      INLINE void O3TypeFactory< GUM_SCALAR >::_buildRealTypes_() {
        if (_checkO3RealTypes_()) {
          PRMFactory< GUM_SCALAR > factory(_prm_);
          for (auto type: _o3RealTypes_) {
            factory.startDiscretizedType(type->name().label());

            for (auto value: type->values()) {
              factory.addTick(value.value());
            }
            factory.endDiscretizedType();
          }
        }
      }

      template < typename GUM_SCALAR >
      INLINE bool O3TypeFactory< GUM_SCALAR >::_checkO3RealTypes_() {
        auto names = gum::Set< std::string >();
        for (auto& type: _o3_prm_->types()) {
          names.insert(type->name().label());
        }
        for (auto& type: _o3_prm_->int_types()) {
          names.insert(type->name().label());
        }

        for (const auto& type: _o3_prm_->real_types()) {
          if (names.contains(type->name().label())) {
            // Raised if duplicate type names
            O3PRM_TYPE_DUPPLICATE(type->name(), *_errors_);
            return false;

          } else if (type->values().size() < 3) {
            // Invalid range
            O3PRM_TYPE_INVALID_RANGE(*type, *_errors_);
            return false;

          } else {
            _o3RealTypes_.push_back(type.get());
          }
        }
        return true;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3TypeFactory< GUM_SCALAR >::_isPrimitiveType_(O3Type& type) {
        return type.name().label() == "boolean";
      }

      template < typename GUM_SCALAR >
      INLINE bool O3TypeFactory< GUM_SCALAR >::_checkO3Types_() {
        if (_addTypes2Dag_()) {
          if (_addArcs2Dag_()) { return true; }
        }
        return false;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3TypeFactory< GUM_SCALAR >::_addTypes2Dag_() {
        // Adding nodes to the type inheritance graph
        for (auto& type: _o3_prm_->types()) {
          auto id = _dag_.addNode();
          try {
            _nameMap_.insert(type->name().label(), id);
            _typeMap_.insert(type->name().label(), type.get());
            _nodeMap_.insert(id, type.get());

          } catch (DuplicateElement const&) {
            // Raised if duplicate type names
            O3PRM_TYPE_DUPPLICATE(type->name(), *_errors_);
            return false;
          }
        }
        return true;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3TypeFactory< GUM_SCALAR >::_addArcs2Dag_() {
        // Adding arcs to the graph inheritance graph
        for (auto& type: _o3_prm_->types()) {
          if (type->superLabel().label() != "") {
            if (!_solver_->resolveType(type->superLabel())) { return false; }

            auto head = _nameMap_[type->superLabel().label()];
            auto tail = _nameMap_[type->name().label()];

            try {
              _dag_.addArc(tail, head);

            } catch (InvalidDirectedCycle const&) {
              // Cyclic inheritance
              O3PRM_TYPE_CYCLIC_INHERITANCE(type->name(), type->superLabel(), *_errors_);
              return false;
            }

            // Check labels inheritance
            if (!_checkLabels_(*type)) { return false; }
          }
        }

        return true;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3TypeFactory< GUM_SCALAR >::_checkLabels_(O3Type& type) {
        for (auto& pair: type.labels()) {
          auto super_labels = Set< std::string >();
          auto super        = _typeMap_[type.superLabel().label()];

          for (auto& label: super->labels()) {
            super_labels.insert(label.first.label());
          }

          if (!super_labels.contains(pair.second.label())) {
            O3PRM_TYPE_UNKNOWN_LABEL(type.superLabel(), pair.second, *_errors_);
            return false;
          }
        }
        return true;
      }

      template < typename GUM_SCALAR >
      INLINE void O3TypeFactory< GUM_SCALAR >::_setO3TypeCreationOrder_() {
        auto v = _dag_.topologicalOrder();

        for (auto id = v.rbegin(); id != v.rend(); --id) {
          if (_nodeMap_[*id]->name().label() != "boolean") { _o3Types_.push_back(_nodeMap_[*id]); }
        }
      }

      template < typename GUM_SCALAR >
      INLINE bool O3TypeFactory< GUM_SCALAR >::_checkO3IntTypes_() {
        auto names = gum::Set< std::string >();
        for (auto& type: _o3_prm_->types()) {
          names.insert(type->name().label());
        }

        for (const auto& type: _o3_prm_->int_types()) {
          if (names.contains(type->name().label())) {
            // Raised if duplicate type names
            O3PRM_TYPE_DUPPLICATE(type->name(), *_errors_);
            return false;

          } else if (type->end().value() - type->start().value() < 1) {
            // Invalid range
            O3PRM_TYPE_INVALID_RANGE(*type, *_errors_);
            return false;

          } else {
            _o3IntTypes_.push_back(type.get());
          }
        }
        return true;
      }

    }   // namespace o3prm
  }     // namespace prm
}   // namespace gum
