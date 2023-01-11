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
 * @brief Implementation for the O3InterfaceFactory class.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 * @author Lionel TORTI
 */

#include <agrum/PRM/o3prm/O3InterfaceFactory.h>

namespace gum {
  namespace prm {
    namespace o3prm {

      template < typename GUM_SCALAR >
      INLINE
         O3InterfaceFactory< GUM_SCALAR >::O3InterfaceFactory(PRM< GUM_SCALAR >&          prm,
                                                              O3PRM&                      o3_prm,
                                                              O3NameSolver< GUM_SCALAR >& solver,
                                                              ErrorsContainer&            errors) :
          _prm_(&prm),
          _o3_prm_(&o3_prm), _solver_(&solver), _errors_(&errors) {
        GUM_CONSTRUCTOR(O3InterfaceFactory);
      }

      template < typename GUM_SCALAR >
      INLINE O3InterfaceFactory< GUM_SCALAR >::O3InterfaceFactory(
         const O3InterfaceFactory< GUM_SCALAR >& src) :
          _prm_(src._prm_),
          _o3_prm_(src._o3_prm_), _solver_(src._solver_), _errors_(src._errors_) {
        GUM_CONS_CPY(O3InterfaceFactory);
      }

      template < typename GUM_SCALAR >
      INLINE O3InterfaceFactory< GUM_SCALAR >::O3InterfaceFactory(
         O3InterfaceFactory< GUM_SCALAR >&& src) :
          _prm_(std::move(src._prm_)),
          _o3_prm_(std::move(src._o3_prm_)), _solver_(std::move(src._solver_)),
          _errors_(std::move(src._errors_)) {
        GUM_CONS_MOV(O3InterfaceFactory);
      }

      template < typename GUM_SCALAR >
      INLINE O3InterfaceFactory< GUM_SCALAR >::~O3InterfaceFactory() {
        GUM_DESTRUCTOR(O3InterfaceFactory);
      }

      template < typename GUM_SCALAR >
      INLINE O3InterfaceFactory< GUM_SCALAR >&
         O3InterfaceFactory< GUM_SCALAR >::operator=(const O3InterfaceFactory< GUM_SCALAR >& src) {
        if (this == &src) { return *this; }
        _prm_    = src._prm_;
        _o3_prm_ = src._o3_prm_;
        _solver_ = src._solver_;
        _errors_ = src._errors_;
        return *this;
      }

      template < typename GUM_SCALAR >
      INLINE O3InterfaceFactory< GUM_SCALAR >&
         O3InterfaceFactory< GUM_SCALAR >::operator=(O3InterfaceFactory< GUM_SCALAR >&& src) {
        if (this == &src) { return *this; }
        _prm_    = std::move(src._prm_);
        _o3_prm_ = std::move(src._o3_prm_);
        _solver_ = std::move(src._solver_);
        _errors_ = std::move(src._errors_);
        return *this;
      }

      template < typename GUM_SCALAR >
      INLINE void O3InterfaceFactory< GUM_SCALAR >::buildInterfaces() {
        PRMFactory< GUM_SCALAR > factory(_prm_);
        if (_checkO3Interfaces_()) {
          _setO3InterfaceCreationOrder_();

          for (auto i: _o3Interface_) {
            if (_solver_->resolveInterface(i->superLabel())) {
              factory.startInterface(i->name().label(), i->superLabel().label(), true);
              factory.endInterface();
            }
          }
        }
      }

      template < typename GUM_SCALAR >
      INLINE bool O3InterfaceFactory< GUM_SCALAR >::_checkO3Interfaces_() {
        return _addInterface2Dag_() && _addArcs2Dag_();
      }

      template < typename GUM_SCALAR >
      INLINE bool O3InterfaceFactory< GUM_SCALAR >::_addInterface2Dag_() {
        // Adding nodes to the type inheritance graph
        for (auto& i: _o3_prm_->interfaces()) {
          auto id = _dag_.addNode();
          try {
            _nameMap_.insert(i->name().label(), id);
            _interfaceMap_.insert(i->name().label(), i.get());
            _nodeMap_.insert(id, i.get());

          } catch (DuplicateElement const&) {
            // Raised if duplicate type names
            O3PRM_INTERFACE_DUPLICATE(i->name(), *_errors_);
            return false;
          }
        }
        return true;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3InterfaceFactory< GUM_SCALAR >::_addArcs2Dag_() {
        // Adding arcs to the graph inheritance graph
        for (auto& i: _o3_prm_->interfaces()) {
          if (i->superLabel().label() != "") {
            if (!_solver_->resolveInterface(i->superLabel())) { return false; }

            auto head = _nameMap_[i->superLabel().label()];
            auto tail = _nameMap_[i->name().label()];

            try {
              _dag_.addArc(tail, head);

            } catch (InvalidDirectedCycle const&) {
              // Cyclic inheritance
              O3PRM_INTERFACE_CYCLIC_INHERITANCE(i->name(), i->superLabel(), *_errors_);
              return false;
            }
          }
        }
        return true;
      }

      template < typename GUM_SCALAR >
      INLINE void O3InterfaceFactory< GUM_SCALAR >::_setO3InterfaceCreationOrder_() {
        auto topo_order = _dag_.topologicalOrder();
        for (auto id = topo_order.rbegin(); id != topo_order.rend(); --id) {
          _o3Interface_.push_back(_nodeMap_[*id]);
        }
      }

      template < typename GUM_SCALAR >
      INLINE void O3InterfaceFactory< GUM_SCALAR >::buildElements() {
        PRMFactory< GUM_SCALAR > factory(_prm_);

        for (auto i: _o3Interface_) {
          _prm_->getInterface(i->name().label()).inheritInterface();

          factory.continueInterface(i->name().label());

          for (auto& elt: i->elements()) {
            if (_checkInterfaceElement_(*i, elt)) {
              try {
                if (_prm_->isType(elt.type().label())) {
                  factory.addAttribute(elt.type().label(), elt.name().label());
                } else {
                  factory.addReferenceSlot(elt.type().label(), elt.name().label(), elt.isArray());
                }

              } catch (OperationNotAllowed const&) {
                // Duplicate or Wrong overload
                O3PRM_INTERFACE_DUPLICATE_ELEMENT(elt, *_errors_);
              }
            }
          }
          factory.endInterface();
        }
      }

      template < typename GUM_SCALAR >
      INLINE bool
         O3InterfaceFactory< GUM_SCALAR >::_checkInterfaceElement_(O3Interface&        i,
                                                                   O3InterfaceElement& elt) {
        if (!_solver_->resolveClassElement(elt.type())) { return false; }

        if (_prm_->isType(elt.type().label()) && elt.isArray()) {
          O3PRM_INTERFACE_ILLEGAL_ARRAY(elt.name(), *_errors_);
          return false;
        }

        const auto& real_i = _prm_->getInterface(i.name().label());

        if (real_i.exists(elt.name().label())) {
          if (!_checkOverloadLegality_(i, elt)) { return false; }
        }

        if (!_checkCyclicReference_(i, elt)) { return false; }

        return true;
      }

      template < typename GUM_SCALAR >
      INLINE bool
         O3InterfaceFactory< GUM_SCALAR >::_checkOverloadLegality_(O3Interface&        i,
                                                                   O3InterfaceElement& elt) {
        const auto& real_i   = _prm_->getInterface(i.name().label());
        const auto& real_elt = real_i.get(elt.name().label());

        if (PRMClassElement< GUM_SCALAR >::isAttribute(real_elt)) {
          return _checkAttributeOverloadLegality_(i, elt);
        }

        if (PRMClassElement< GUM_SCALAR >::isReferenceSlot(real_elt)) {
          return _checkReferenceOverloadLegality_(i, elt);
        }

        return false;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3InterfaceFactory< GUM_SCALAR >::_checkAttributeOverloadLegality_(
         O3Interface&        i,
         O3InterfaceElement& elt) {
        const auto& real_i   = _prm_->getInterface(i.name().label());
        const auto& real_elt = real_i.get(elt.name().label());

        const auto& sub_type   = _prm_->type(elt.type().label());
        const auto& super_type = real_elt.type();

        if (!sub_type.isSubTypeOf(super_type)) {
          O3PRM_INTERFACE_ILLEGAL_OVERLOAD(elt, *_errors_);
          return false;
        }

        if (sub_type.name() == super_type.name()) {
          O3PRM_INTERFACE_DUPLICATE_ELEMENT(elt, *_errors_);
          return false;
        }

        return true;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3InterfaceFactory< GUM_SCALAR >::_checkReferenceOverloadLegality_(
         O3Interface&        i,
         O3InterfaceElement& elt) {
        const auto& real_i = _prm_->getInterface(i.name().label());
        const auto& real_elt
           = static_cast< const PRMReferenceSlot< GUM_SCALAR >& >(real_i.get(elt.name().label()));

        auto sub_type = (const PRMClassElementContainer< GUM_SCALAR >*)nullptr;

        if (_prm_->isClass(elt.type().label())) {
          sub_type = &(_prm_->getClass(elt.type().label()));
        } else {
          sub_type = &(_prm_->getInterface(elt.type().label()));
        }

        auto super_type = &(real_elt.slotType());

        if (!sub_type->isSubTypeOf(*super_type)) {
          O3PRM_INTERFACE_ILLEGAL_OVERLOAD(elt, *_errors_);
          return false;
        }

        if (sub_type->name() == super_type->name()) {
          O3PRM_INTERFACE_DUPLICATE_ELEMENT(elt, *_errors_);
          return false;
        }

        return true;
      }

      template < typename GUM_SCALAR >
      INLINE bool
         O3InterfaceFactory< GUM_SCALAR >::_checkCyclicReference_(O3Interface&        i,
                                                                  O3InterfaceElement& elt) {
        if (_prm_->isInterface(elt.type().label()) || _prm_->isClass(elt.type().label())) {
          auto ref_type = (const PRMClassElementContainer< GUM_SCALAR >*)nullptr;

          if (_prm_->isInterface(elt.type().label())) {
            ref_type = &(_prm_->getInterface(elt.type().label()));
          } else {
            ref_type = &(_prm_->getClass(elt.type().label()));
          }

          const auto& real_i = _prm_->getInterface(i.name().label());

          if (&real_i == ref_type) {
            O3PRM_INTERFACE_SELF_REFERENCE(i, elt, *_errors_);
            return false;
          }

          if (ref_type->isSubTypeOf(real_i)) {
            O3PRM_INTERFACE_ILLEGAL_SUB_REFERENCE(i, elt, *_errors_);
            return false;
          }
        }

        return true;
      }

    }   // namespace o3prm
  }     // namespace prm
}   // namespace gum
