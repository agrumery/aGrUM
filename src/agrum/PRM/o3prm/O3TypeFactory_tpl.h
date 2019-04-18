
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
 * @brief Implementation for the O3TypeFactory class.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 * @author Lionel TORTI
 */

#include <agrum/PRM/o3prm/O3TypeFactory.h>

namespace gum {
  namespace prm {
    namespace o3prm {

      template < typename GUM_SCALAR >
      INLINE O3TypeFactory< GUM_SCALAR >::O3TypeFactory(
         PRM< GUM_SCALAR >&          prm,
         O3PRM&                      o3_prm,
         O3NameSolver< GUM_SCALAR >& solver,
         ErrorsContainer&            errors) :
          __prm(&prm),
          __o3_prm(&o3_prm), __solver(&solver), __errors(&errors), __build(false) {
        GUM_CONSTRUCTOR(O3TypeFactory);
      }

      template < typename GUM_SCALAR >
      INLINE O3TypeFactory< GUM_SCALAR >::O3TypeFactory(
         const O3TypeFactory< GUM_SCALAR >& src) :
          __prm(src.__prm),
          __o3_prm(src.__o3_prm), __solver(src.__solver), __errors(src.__errors),
          __build(src.__build) {
        GUM_CONS_CPY(O3TypeFactory);
      }

      template < typename GUM_SCALAR >
      INLINE O3TypeFactory< GUM_SCALAR >::O3TypeFactory(
         O3TypeFactory< GUM_SCALAR >&& src) :
          __prm(std::move(src.__prm)),
          __o3_prm(std::move(src.__o3_prm)), __solver(std::move(src.__solver)),
          __errors(std::move(src.__errors)), __build(std::move(src.__build)) {
        GUM_CONS_MOV(O3TypeFactory);
      }

      template < typename GUM_SCALAR >
      INLINE O3TypeFactory< GUM_SCALAR >::~O3TypeFactory() {
        GUM_DESTRUCTOR(O3TypeFactory);
      }

      template < typename GUM_SCALAR >
      INLINE O3TypeFactory< GUM_SCALAR >& O3TypeFactory< GUM_SCALAR >::
                                          operator=(const O3TypeFactory< GUM_SCALAR >& src) {
        if (this == &src) { return *this; }
        __prm = src.__prm;
        __o3_prm = src.__o3_prm;
        __solver = src.__solver;
        __errors = src.__errors;
        __build = src.__build;
        return *this;
      }

      template < typename GUM_SCALAR >
      INLINE O3TypeFactory< GUM_SCALAR >& O3TypeFactory< GUM_SCALAR >::
                                          operator=(O3TypeFactory< GUM_SCALAR >&& src) {
        if (this == &src) { return *this; }
        __prm = std::move(src.__prm);
        __o3_prm = std::move(src.__o3_prm);
        __solver = std::move(src.__solver);
        __errors = std::move(src.__errors);
        __build = std::move(src.__build);
        return *this;
      }

      template < typename GUM_SCALAR >
      INLINE void O3TypeFactory< GUM_SCALAR >::build() {
        if (__build) {
          GUM_ERROR(FatalError,
                    "types have already been built, change PRM to rebuild");
        }
        __build = true;

        // checking deprecated declarations
        __checkDepreactedO3Types();

        // building types
        __buildTypes();

        // building int types
        __buildIntTypes();

        // building real types
        __buildRealTypes();
      }

      template < typename GUM_SCALAR >
      INLINE void O3TypeFactory< GUM_SCALAR >::__buildTypes() {
        if (__checkO3Types()) {
          __setO3TypeCreationOrder();

          PRMFactory< GUM_SCALAR > factory(__prm);
          for (auto type : __o3Types) {
            if (!__isPrimitiveType(*type)) {
              if (__solver->resolveType(type->superLabel())) {
                factory.startDiscreteType(type->name().label(),
                                          type->superLabel().label());

                for (auto& label : type->labels()) {
                  factory.addLabel(label.first.label(), label.second.label());
                }

                factory.endDiscreteType();
              }
            }
          }
        }
      }

      template < typename GUM_SCALAR >
      INLINE void O3TypeFactory< GUM_SCALAR >::__checkDepreactedO3Types() {
        for (auto& t : __o3_prm->types()) {
          if (t->deprecated()) {
            O3PRM_DEPRECATED_TYPE_WARNING(t->name(), *__errors);
          }
        }

        for (auto& t : __o3_prm->int_types()) {
          if (t->deprecated()) {
            O3PRM_DEPRECATED_TYPE_WARNING(t->name(), *__errors);
          }
        }

        for (auto& t : __o3_prm->real_types()) {
          if (t->deprecated()) {
            O3PRM_DEPRECATED_TYPE_WARNING(t->name(), *__errors);
          }
        }
      }

      template < typename GUM_SCALAR >
      INLINE void O3TypeFactory< GUM_SCALAR >::__buildIntTypes() {
        if (__checkO3IntTypes()) {
          PRMFactory< GUM_SCALAR > factory(__prm);
          for (auto type : __o3IntTypes) {
            factory.addRangeType(
               type->name().label(), type->start().value(), type->end().value());
          }
        }
      }

      template < typename GUM_SCALAR >
      INLINE void O3TypeFactory< GUM_SCALAR >::__buildRealTypes() {
        if (__checkO3RealTypes()) {
          PRMFactory< GUM_SCALAR > factory(__prm);
          for (auto type : __o3RealTypes) {
            factory.startDiscretizedType(type->name().label());

            for (auto value : type->values()) {
              factory.addTick(value.value());
            }
            factory.endDiscretizedType();
          }
        }
      }

      template < typename GUM_SCALAR >
      INLINE bool O3TypeFactory< GUM_SCALAR >::__checkO3RealTypes() {
        auto names = gum::Set< std::string >();
        for (auto& type : __o3_prm->types()) {
          names.insert(type->name().label());
        }
        for (auto& type : __o3_prm->int_types()) {
          names.insert(type->name().label());
        }

        for (const auto& type : __o3_prm->real_types()) {
          if (names.contains(type->name().label())) {
            // Raised if duplicate type names
            O3PRM_TYPE_DUPPLICATE(type->name(), *__errors);
            return false;

          } else if (type->values().size() < 3) {
            // Invalid range
            O3PRM_TYPE_INVALID_RANGE(*type, *__errors);
            return false;

          } else {
            __o3RealTypes.push_back(type.get());
          }
        }
        return true;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3TypeFactory< GUM_SCALAR >::__isPrimitiveType(O3Type& type) {
        return type.name().label() == "boolean";
      }

      template < typename GUM_SCALAR >
      INLINE bool O3TypeFactory< GUM_SCALAR >::__checkO3Types() {
        if (__addTypes2Dag()) {
          if (__addArcs2Dag()) { return true; }
        }
        return false;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3TypeFactory< GUM_SCALAR >::__addTypes2Dag() {
        // Adding nodes to the type inheritance graph
        for (auto& type : __o3_prm->types()) {
          auto id = __dag.addNode();
          try {
            __nameMap.insert(type->name().label(), id);
            __typeMap.insert(type->name().label(), type.get());
            __nodeMap.insert(id, type.get());

          } catch (DuplicateElement&) {
            // Raised if duplicate type names
            O3PRM_TYPE_DUPPLICATE(type->name(), *__errors);
            return false;
          }
        }
        return true;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3TypeFactory< GUM_SCALAR >::__addArcs2Dag() {
        // Adding arcs to the graph inheritance graph
        for (auto& type : __o3_prm->types()) {
          if (type->superLabel().label() != "") {
            if (!__solver->resolveType(type->superLabel())) { return false; }

            auto head = __nameMap[type->superLabel().label()];
            auto tail = __nameMap[type->name().label()];

            try {
              __dag.addArc(tail, head);

            } catch (InvalidDirectedCycle&) {
              // Cyclic inheritance
              O3PRM_TYPE_CYCLIC_INHERITANCE(
                 type->name(), type->superLabel(), *__errors);
              return false;
            }

            // Check labels inheritance
            if (!__checkLabels(*type)) { return false; }
          }
        }

        return true;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3TypeFactory< GUM_SCALAR >::__checkLabels(O3Type& type) {
        for (auto& pair : type.labels()) {
          auto super_labels = Set< std::string >();
          auto super = __typeMap[type.superLabel().label()];

          for (auto& label : super->labels()) {
            super_labels.insert(label.first.label());
          }

          if (!super_labels.contains(pair.second.label())) {
            O3PRM_TYPE_UNKNOWN_LABEL(type.superLabel(), pair.second, *__errors);
            return false;
          }
        }
        return true;
      }

      template < typename GUM_SCALAR >
      INLINE void O3TypeFactory< GUM_SCALAR >::__setO3TypeCreationOrder() {
        auto v = __dag.topologicalOrder();

        for (auto id = v.rbegin(); id != v.rend(); --id) {
          if (__nodeMap[*id]->name().label() != "boolean") {
            __o3Types.push_back(__nodeMap[*id]);
          }
        }
      }

      template < typename GUM_SCALAR >
      INLINE bool O3TypeFactory< GUM_SCALAR >::__checkO3IntTypes() {
        auto names = gum::Set< std::string >();
        for (auto& type : __o3_prm->types()) {
          names.insert(type->name().label());
        }

        for (const auto& type : __o3_prm->int_types()) {
          if (names.contains(type->name().label())) {
            // Raised if duplicate type names
            O3PRM_TYPE_DUPPLICATE(type->name(), *__errors);
            return false;

          } else if (type->end().value() - type->start().value() < 1) {
            // Invalid range
            O3PRM_TYPE_INVALID_RANGE(*type, *__errors);
            return false;

          } else {
            __o3IntTypes.push_back(type.get());
          }
        }
        return true;
      }

    }   // namespace o3prm
  }     // namespace prm
}   // namespace gum
