/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 * @author Lionel TORTI
 */

#include <agrum/PRM/o3prm/O3InterfaceFactory.h>

namespace gum {
  namespace prm {
    namespace o3prm {

      template < typename GUM_SCALAR >
      INLINE O3InterfaceFactory< GUM_SCALAR >::O3InterfaceFactory(
         PRM< GUM_SCALAR >&          prm,
         O3PRM&                      o3_prm,
         O3NameSolver< GUM_SCALAR >& solver,
         ErrorsContainer&            errors) :
          prm__(&prm),
          o3_prm__(&o3_prm), solver__(&solver), errors__(&errors) {
        GUM_CONSTRUCTOR(O3InterfaceFactory);
      }

      template < typename GUM_SCALAR >
      INLINE O3InterfaceFactory< GUM_SCALAR >::O3InterfaceFactory(
         const O3InterfaceFactory< GUM_SCALAR >& src) :
          prm__(src.prm__),
          o3_prm__(src.o3_prm__), solver__(src.solver__), errors__(src.errors__) {
        GUM_CONS_CPY(O3InterfaceFactory);
      }

      template < typename GUM_SCALAR >
      INLINE O3InterfaceFactory< GUM_SCALAR >::O3InterfaceFactory(
         O3InterfaceFactory< GUM_SCALAR >&& src) :
          prm__(std::move(src.prm__)),
          o3_prm__(std::move(src.o3_prm__)), solver__(std::move(src.solver__)),
          errors__(std::move(src.errors__)) {
        GUM_CONS_MOV(O3InterfaceFactory);
      }

      template < typename GUM_SCALAR >
      INLINE O3InterfaceFactory< GUM_SCALAR >::~O3InterfaceFactory() {
        GUM_DESTRUCTOR(O3InterfaceFactory);
      }

      template < typename GUM_SCALAR >
      INLINE O3InterfaceFactory< GUM_SCALAR >&
         O3InterfaceFactory< GUM_SCALAR >::operator=(
            const O3InterfaceFactory< GUM_SCALAR >& src) {
        if (this == &src) { return *this; }
        prm__    = src.prm__;
        o3_prm__ = src.o3_prm__;
        solver__ = src.solver__;
        errors__ = src.errors__;
        return *this;
      }

      template < typename GUM_SCALAR >
      INLINE O3InterfaceFactory< GUM_SCALAR >&
         O3InterfaceFactory< GUM_SCALAR >::operator=(
            O3InterfaceFactory< GUM_SCALAR >&& src) {
        if (this == &src) { return *this; }
        prm__    = std::move(src.prm__);
        o3_prm__ = std::move(src.o3_prm__);
        solver__ = std::move(src.solver__);
        errors__ = std::move(src.errors__);
        return *this;
      }

      template < typename GUM_SCALAR >
      INLINE void O3InterfaceFactory< GUM_SCALAR >::buildInterfaces() {
        PRMFactory< GUM_SCALAR > factory(prm__);
        if (checkO3Interfaces__()) {
          setO3InterfaceCreationOrder__();

          for (auto i: o3Interface__) {
            if (solver__->resolveInterface(i->superLabel())) {
              factory.startInterface(i->name().label(),
                                     i->superLabel().label(),
                                     true);
              factory.endInterface();
            }
          }
        }
      }

      template < typename GUM_SCALAR >
      INLINE bool O3InterfaceFactory< GUM_SCALAR >::checkO3Interfaces__() {
        return addInterface2Dag__() && addArcs2Dag__();
      }

      template < typename GUM_SCALAR >
      INLINE bool O3InterfaceFactory< GUM_SCALAR >::addInterface2Dag__() {
        // Adding nodes to the type inheritance graph
        for (auto& i: o3_prm__->interfaces()) {
          auto id = dag__.addNode();
          try {
            nameMap__.insert(i->name().label(), id);
            interfaceMap__.insert(i->name().label(), i.get());
            nodeMap__.insert(id, i.get());

          } catch (DuplicateElement&) {
            // Raised if duplicate type names
            O3PRM_INTERFACE_DUPLICATE(i->name(), *errors__);
            return false;
          }
        }
        return true;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3InterfaceFactory< GUM_SCALAR >::addArcs2Dag__() {
        // Adding arcs to the graph inheritance graph
        for (auto& i: o3_prm__->interfaces()) {
          if (i->superLabel().label() != "") {
            if (!solver__->resolveInterface(i->superLabel())) { return false; }

            auto head = nameMap__[i->superLabel().label()];
            auto tail = nameMap__[i->name().label()];

            try {
              dag__.addArc(tail, head);

            } catch (InvalidDirectedCycle&) {
              // Cyclic inheritance
              O3PRM_INTERFACE_CYCLIC_INHERITANCE(i->name(),
                                                 i->superLabel(),
                                                 *errors__);
              return false;
            }
          }
        }
        return true;
      }

      template < typename GUM_SCALAR >
      INLINE void
         O3InterfaceFactory< GUM_SCALAR >::setO3InterfaceCreationOrder__() {
        auto topo_order = dag__.topologicalOrder();
        for (auto id = topo_order.rbegin(); id != topo_order.rend(); --id) {
          o3Interface__.push_back(nodeMap__[*id]);
        }
      }

      template < typename GUM_SCALAR >
      INLINE void O3InterfaceFactory< GUM_SCALAR >::buildElements() {
        PRMFactory< GUM_SCALAR > factory(prm__);

        for (auto i: o3Interface__) {
          prm__->getInterface(i->name().label()).inheritInterface();

          factory.continueInterface(i->name().label());

          for (auto& elt: i->elements()) {
            if (checkInterfaceElement__(*i, elt)) {
              try {
                if (prm__->isType(elt.type().label())) {
                  factory.addAttribute(elt.type().label(), elt.name().label());
                } else {
                  factory.addReferenceSlot(elt.type().label(),
                                           elt.name().label(),
                                           elt.isArray());
                }

              } catch (OperationNotAllowed&) {
                // Duplicate or Wrong overload
                O3PRM_INTERFACE_DUPLICATE_ELEMENT(elt, *errors__);
              }
            }
          }
          factory.endInterface();
        }
      }

      template < typename GUM_SCALAR >
      INLINE bool O3InterfaceFactory< GUM_SCALAR >::checkInterfaceElement__(
         O3Interface&        i,
         O3InterfaceElement& elt) {
        if (!solver__->resolveClassElement(elt.type())) { return false; }

        if (prm__->isType(elt.type().label()) && elt.isArray()) {
          O3PRM_INTERFACE_ILLEGAL_ARRAY(elt.name(), *errors__);
          return false;
        }

        const auto& real_i = prm__->getInterface(i.name().label());

        if (real_i.exists(elt.name().label())) {
          if (!checkOverloadLegality__(i, elt)) { return false; }
        }

        if (!checkCyclicReference__(i, elt)) { return false; }

        return true;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3InterfaceFactory< GUM_SCALAR >::checkOverloadLegality__(
         O3Interface&        i,
         O3InterfaceElement& elt) {
        const auto& real_i   = prm__->getInterface(i.name().label());
        const auto& real_elt = real_i.get(elt.name().label());

        if (PRMClassElement< GUM_SCALAR >::isAttribute(real_elt)) {
          return checkAttributeOverloadLegality__(i, elt);
        }

        if (PRMClassElement< GUM_SCALAR >::isReferenceSlot(real_elt)) {
          return checkReferenceOverloadLegality__(i, elt);
        }

        return false;
      }

      template < typename GUM_SCALAR >
      INLINE bool
         O3InterfaceFactory< GUM_SCALAR >::checkAttributeOverloadLegality__(
            O3Interface&        i,
            O3InterfaceElement& elt) {
        const auto& real_i   = prm__->getInterface(i.name().label());
        const auto& real_elt = real_i.get(elt.name().label());

        const auto& sub_type   = prm__->type(elt.type().label());
        const auto& super_type = real_elt.type();

        if (!sub_type.isSubTypeOf(super_type)) {
          O3PRM_INTERFACE_ILLEGAL_OVERLOAD(elt, *errors__);
          return false;
        }

        if (sub_type.name() == super_type.name()) {
          O3PRM_INTERFACE_DUPLICATE_ELEMENT(elt, *errors__);
          return false;
        }

        return true;
      }

      template < typename GUM_SCALAR >
      INLINE bool
         O3InterfaceFactory< GUM_SCALAR >::checkReferenceOverloadLegality__(
            O3Interface&        i,
            O3InterfaceElement& elt) {
        const auto& real_i = prm__->getInterface(i.name().label());
        const auto& real_elt
           = static_cast< const PRMReferenceSlot< GUM_SCALAR >& >(
              real_i.get(elt.name().label()));

        auto sub_type = (const PRMClassElementContainer< GUM_SCALAR >*)nullptr;

        if (prm__->isClass(elt.type().label())) {
          sub_type = &(prm__->getClass(elt.type().label()));
        } else {
          sub_type = &(prm__->getInterface(elt.type().label()));
        }

        auto super_type = &(real_elt.slotType());

        if (!sub_type->isSubTypeOf(*super_type)) {
          O3PRM_INTERFACE_ILLEGAL_OVERLOAD(elt, *errors__);
          return false;
        }

        if (sub_type->name() == super_type->name()) {
          O3PRM_INTERFACE_DUPLICATE_ELEMENT(elt, *errors__);
          return false;
        }

        return true;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3InterfaceFactory< GUM_SCALAR >::checkCyclicReference__(
         O3Interface&        i,
         O3InterfaceElement& elt) {
        if (prm__->isInterface(elt.type().label())
            || prm__->isClass(elt.type().label())) {
          auto ref_type = (const PRMClassElementContainer< GUM_SCALAR >*)nullptr;

          if (prm__->isInterface(elt.type().label())) {
            ref_type = &(prm__->getInterface(elt.type().label()));
          } else {
            ref_type = &(prm__->getClass(elt.type().label()));
          }

          const auto& real_i = prm__->getInterface(i.name().label());

          if (&real_i == ref_type) {
            O3PRM_INTERFACE_SELF_REFERENCE(i, elt, *errors__);
            return false;
          }

          if (ref_type->isSubTypeOf(real_i)) {
            O3PRM_INTERFACE_ILLEGAL_SUB_REFERENCE(i, elt, *errors__);
            return false;
          }
        }

        return true;
      }

    }   // namespace o3prm
  }     // namespace prm
}   // namespace gum
