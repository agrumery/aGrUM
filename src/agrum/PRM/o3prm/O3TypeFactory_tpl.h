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
 * @brief Implementation for the O3TypeFactory class.
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
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
          prm__(&prm),
          o3_prm__(&o3_prm), solver__(&solver), errors__(&errors), build__(false) {
        GUM_CONSTRUCTOR(O3TypeFactory);
      }

      template < typename GUM_SCALAR >
      INLINE O3TypeFactory< GUM_SCALAR >::O3TypeFactory(
         const O3TypeFactory< GUM_SCALAR >& src) :
          prm__(src.prm__),
          o3_prm__(src.o3_prm__), solver__(src.solver__), errors__(src.errors__),
          build__(src.build__) {
        GUM_CONS_CPY(O3TypeFactory);
      }

      template < typename GUM_SCALAR >
      INLINE O3TypeFactory< GUM_SCALAR >::O3TypeFactory(
         O3TypeFactory< GUM_SCALAR >&& src) :
          prm__(std::move(src.prm__)),
          o3_prm__(std::move(src.o3_prm__)), solver__(std::move(src.solver__)),
          errors__(std::move(src.errors__)), build__(std::move(src.build__)) {
        GUM_CONS_MOV(O3TypeFactory);
      }

      template < typename GUM_SCALAR >
      INLINE O3TypeFactory< GUM_SCALAR >::~O3TypeFactory() {
        GUM_DESTRUCTOR(O3TypeFactory);
      }

      template < typename GUM_SCALAR >
      INLINE O3TypeFactory< GUM_SCALAR >& O3TypeFactory< GUM_SCALAR >::operator=(
         const O3TypeFactory< GUM_SCALAR >& src) {
        if (this == &src) { return *this; }
        prm__ = src.prm__;
        o3_prm__ = src.o3_prm__;
        solver__ = src.solver__;
        errors__ = src.errors__;
        build__ = src.build__;
        return *this;
      }

      template < typename GUM_SCALAR >
      INLINE O3TypeFactory< GUM_SCALAR >& O3TypeFactory< GUM_SCALAR >::operator=(
         O3TypeFactory< GUM_SCALAR >&& src) {
        if (this == &src) { return *this; }
        prm__ = std::move(src.prm__);
        o3_prm__ = std::move(src.o3_prm__);
        solver__ = std::move(src.solver__);
        errors__ = std::move(src.errors__);
        build__ = std::move(src.build__);
        return *this;
      }

      template < typename GUM_SCALAR >
      INLINE void O3TypeFactory< GUM_SCALAR >::build() {
        if (build__) {
          GUM_ERROR(FatalError,
                    "types have already been built, change PRM to rebuild");
        }
        build__ = true;

        // checking deprecated declarations
        checkDepreactedO3Types__();

        // building types
        buildTypes__();

        // building int types
        buildIntTypes__();

        // building real types
        buildRealTypes__();
      }

      template < typename GUM_SCALAR >
      INLINE void O3TypeFactory< GUM_SCALAR >::buildTypes__() {
        if (checkO3Types__()) {
          setO3TypeCreationOrder__();

          PRMFactory< GUM_SCALAR > factory(prm__);
          for (auto type: o3Types__) {
            if (!isPrimitiveType__(*type)) {
              if (solver__->resolveType(type->superLabel())) {
                factory.startDiscreteType(type->name().label(),
                                          type->superLabel().label());

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
      INLINE void O3TypeFactory< GUM_SCALAR >::checkDepreactedO3Types__() {
        for (auto& t: o3_prm__->types()) {
          if (t->deprecated()) {
            O3PRM_DEPRECATED_TYPE_WARNING(t->name(), *errors__);
          }
        }

        for (auto& t: o3_prm__->int_types()) {
          if (t->deprecated()) {
            O3PRM_DEPRECATED_TYPE_WARNING(t->name(), *errors__);
          }
        }

        for (auto& t: o3_prm__->real_types()) {
          if (t->deprecated()) {
            O3PRM_DEPRECATED_TYPE_WARNING(t->name(), *errors__);
          }
        }
      }

      template < typename GUM_SCALAR >
      INLINE void O3TypeFactory< GUM_SCALAR >::buildIntTypes__() {
        if (checkO3IntTypes__()) {
          PRMFactory< GUM_SCALAR > factory(prm__);
          for (auto type: o3IntTypes__) {
            factory.addRangeType(
               type->name().label(), type->start().value(), type->end().value());
          }
        }
      }

      template < typename GUM_SCALAR >
      INLINE void O3TypeFactory< GUM_SCALAR >::buildRealTypes__() {
        if (checkO3RealTypes__()) {
          PRMFactory< GUM_SCALAR > factory(prm__);
          for (auto type: o3RealTypes__) {
            factory.startDiscretizedType(type->name().label());

            for (auto value: type->values()) {
              factory.addTick(value.value());
            }
            factory.endDiscretizedType();
          }
        }
      }

      template < typename GUM_SCALAR >
      INLINE bool O3TypeFactory< GUM_SCALAR >::checkO3RealTypes__() {
        auto names = gum::Set< std::string >();
        for (auto& type: o3_prm__->types()) {
          names.insert(type->name().label());
        }
        for (auto& type: o3_prm__->int_types()) {
          names.insert(type->name().label());
        }

        for (const auto& type: o3_prm__->real_types()) {
          if (names.contains(type->name().label())) {
            // Raised if duplicate type names
            O3PRM_TYPE_DUPPLICATE(type->name(), *errors__);
            return false;

          } else if (type->values().size() < 3) {
            // Invalid range
            O3PRM_TYPE_INVALID_RANGE(*type, *errors__);
            return false;

          } else {
            o3RealTypes__.push_back(type.get());
          }
        }
        return true;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3TypeFactory< GUM_SCALAR >::isPrimitiveType__(O3Type& type) {
        return type.name().label() == "boolean";
      }

      template < typename GUM_SCALAR >
      INLINE bool O3TypeFactory< GUM_SCALAR >::checkO3Types__() {
        if (addTypes2Dag__()) {
          if (addArcs2Dag__()) { return true; }
        }
        return false;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3TypeFactory< GUM_SCALAR >::addTypes2Dag__() {
        // Adding nodes to the type inheritance graph
        for (auto& type: o3_prm__->types()) {
          auto id = dag__.addNode();
          try {
            nameMap__.insert(type->name().label(), id);
            typeMap__.insert(type->name().label(), type.get());
            nodeMap__.insert(id, type.get());

          } catch (DuplicateElement&) {
            // Raised if duplicate type names
            O3PRM_TYPE_DUPPLICATE(type->name(), *errors__);
            return false;
          }
        }
        return true;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3TypeFactory< GUM_SCALAR >::addArcs2Dag__() {
        // Adding arcs to the graph inheritance graph
        for (auto& type: o3_prm__->types()) {
          if (type->superLabel().label() != "") {
            if (!solver__->resolveType(type->superLabel())) { return false; }

            auto head = nameMap__[type->superLabel().label()];
            auto tail = nameMap__[type->name().label()];

            try {
              dag__.addArc(tail, head);

            } catch (InvalidDirectedCycle&) {
              // Cyclic inheritance
              O3PRM_TYPE_CYCLIC_INHERITANCE(
                 type->name(), type->superLabel(), *errors__);
              return false;
            }

            // Check labels inheritance
            if (!checkLabels__(*type)) { return false; }
          }
        }

        return true;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3TypeFactory< GUM_SCALAR >::checkLabels__(O3Type& type) {
        for (auto& pair: type.labels()) {
          auto super_labels = Set< std::string >();
          auto super = typeMap__[type.superLabel().label()];

          for (auto& label: super->labels()) {
            super_labels.insert(label.first.label());
          }

          if (!super_labels.contains(pair.second.label())) {
            O3PRM_TYPE_UNKNOWN_LABEL(type.superLabel(), pair.second, *errors__);
            return false;
          }
        }
        return true;
      }

      template < typename GUM_SCALAR >
      INLINE void O3TypeFactory< GUM_SCALAR >::setO3TypeCreationOrder__() {
        auto v = dag__.topologicalOrder();

        for (auto id = v.rbegin(); id != v.rend(); --id) {
          if (nodeMap__[*id]->name().label() != "boolean") {
            o3Types__.push_back(nodeMap__[*id]);
          }
        }
      }

      template < typename GUM_SCALAR >
      INLINE bool O3TypeFactory< GUM_SCALAR >::checkO3IntTypes__() {
        auto names = gum::Set< std::string >();
        for (auto& type: o3_prm__->types()) {
          names.insert(type->name().label());
        }

        for (const auto& type: o3_prm__->int_types()) {
          if (names.contains(type->name().label())) {
            // Raised if duplicate type names
            O3PRM_TYPE_DUPPLICATE(type->name(), *errors__);
            return false;

          } else if (type->end().value() - type->start().value() < 1) {
            // Invalid range
            O3PRM_TYPE_INVALID_RANGE(*type, *errors__);
            return false;

          } else {
            o3IntTypes__.push_back(type.get());
          }
        }
        return true;
      }

    }   // namespace o3prm
  }     // namespace prm
}   // namespace gum
