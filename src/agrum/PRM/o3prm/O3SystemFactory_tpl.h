
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
 * @brief Implementation for the O3SystemFactory class.
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 * @author Lionel TORTI
 */

#include <agrum/PRM/o3prm/O3SystemFactory.h>

namespace gum {
  namespace prm {
    namespace o3prm {

      template < typename GUM_SCALAR >
      INLINE O3SystemFactory< GUM_SCALAR >::O3SystemFactory(
         PRM< GUM_SCALAR >&          prm,
         O3PRM&                      o3_prm,
         O3NameSolver< GUM_SCALAR >& solver,
         ErrorsContainer&            errors) :
          prm__(&prm),
          o3_prm__(&o3_prm), solver__(&solver), errors__(&errors) {
        GUM_CONSTRUCTOR(O3SystemFactory);
      }

      template < typename GUM_SCALAR >
      INLINE O3SystemFactory< GUM_SCALAR >::O3SystemFactory(
         const O3SystemFactory< GUM_SCALAR >& src) :
          prm__(src.prm__),
          o3_prm__(src.o3_prm__), solver__(src.solver__), errors__(src.errors__),
          nameMap__(src.nameMap__) {
        GUM_CONS_CPY(O3SystemFactory);
      }

      template < typename GUM_SCALAR >
      INLINE O3SystemFactory< GUM_SCALAR >::O3SystemFactory(
         O3SystemFactory< GUM_SCALAR >&& src) :
          prm__(std::move(src.prm__)),
          o3_prm__(std::move(src.o3_prm__)), solver__(std::move(src.solver__)),
          errors__(std::move(src.errors__)), nameMap__(std::move(src.nameMap__)) {
        GUM_CONS_MOV(O3SystemFactory);
      }

      template < typename GUM_SCALAR >
      INLINE O3SystemFactory< GUM_SCALAR >::~O3SystemFactory() {
        GUM_DESTRUCTOR(O3SystemFactory);
      }

      template < typename GUM_SCALAR >
      INLINE O3SystemFactory< GUM_SCALAR >&
         O3SystemFactory< GUM_SCALAR >::operator=(
            const O3SystemFactory< GUM_SCALAR >& src) {
        if (this == &src) { return *this; }
        prm__ = src.prm__;
        o3_prm__ = src.o3_prm__;
        solver__ = src.solver__;
        errors__ = src.errors__;
        return *this;
      }

      template < typename GUM_SCALAR >
      INLINE O3SystemFactory< GUM_SCALAR >&
         O3SystemFactory< GUM_SCALAR >::operator=(
            O3SystemFactory< GUM_SCALAR >&& src) {
        if (this == &src) { return *this; }
        prm__ = std::move(src.prm__);
        o3_prm__ = std::move(src.o3_prm__);
        solver__ = std::move(src.solver__);
        errors__ = std::move(src.errors__);
        return *this;
      }

      template < typename GUM_SCALAR >
      INLINE void O3SystemFactory< GUM_SCALAR >::build() {
        PRMFactory< GUM_SCALAR > factory(prm__);

        for (auto& sys: o3_prm__->systems()) {
          // Reseting name map for each system
          nameMap__ = HashTable< std::string, O3Instance* >();

          if (checkSystem__(*sys)) {
            factory.startSystem(sys->name().label());

            addInstances__(factory, *sys);
            addAssignments__(factory, *sys);
            addIncrements__(factory, *sys);

            try {
              factory.endSystem();
            } catch (FatalError&) {
              O3PRM_SYSTEM_INSTANTIATION_FAILED(*sys, *errors__);
            }
          }
        }
      }

      template < typename GUM_SCALAR >
      INLINE void O3SystemFactory< GUM_SCALAR >::addInstances__(
         PRMFactory< GUM_SCALAR >& factory, O3System& sys) {
        for (auto& i: sys.instances()) {
          if (i.parameters().size() > 0) {
            auto params = HashTable< std::string, double >();
            for (auto& p: i.parameters()) {
              params.insert(p.name().label(), (double)p.value().value());
            }
            factory.addInstance(i.type().label(), i.name().label(), params);

          } else {
            if (i.size().value() > 1) {
              factory.addArray(
                 i.type().label(), i.name().label(), i.size().value());
            } else {
              factory.addInstance(i.type().label(), i.name().label());
            }
          }
        }
      }

      template < typename GUM_SCALAR >
      INLINE void O3SystemFactory< GUM_SCALAR >::addAssignments__(
         PRMFactory< GUM_SCALAR >& factory, O3System& sys) {
        const auto& real_sys = prm__->getSystem(sys.name().label());

        for (auto& ass: sys.assignments()) {
          auto leftInstance = ass.leftInstance().label();
          auto leftReference = ass.leftReference().label();
          auto rightInstance = ass.rightInstance().label();

          if (ass.leftIndex().value() > -1 && real_sys.isArray(leftInstance)) {
            std::stringstream sBuff;
            sBuff << leftInstance << "[" << ass.leftIndex().value() << "]";
            leftInstance = sBuff.str();
          }

          if (ass.rightIndex().value() > -1 && real_sys.isArray(rightInstance)) {
            std::stringstream sBuff;
            sBuff << rightInstance << "[" << ass.rightIndex().value() << "]";
            rightInstance = sBuff.str();
          }

          factory.setReferenceSlot(leftInstance, leftReference, rightInstance);
        }
      }

      template < typename GUM_SCALAR >
      INLINE void O3SystemFactory< GUM_SCALAR >::addIncrements__(
         PRMFactory< GUM_SCALAR >& factory, O3System& sys) {
        const auto& real_sys = prm__->getSystem(sys.name().label());
        for (auto& inc: sys.increments()) {
          auto leftInstance = inc.leftInstance().label();
          auto leftReference = inc.leftReference().label();
          auto rightInstance = inc.rightInstance().label();

          if (inc.leftIndex().value() > -1 && real_sys.isArray(leftInstance)) {
            std::stringstream sBuff;
            sBuff << leftInstance << "[" << inc.leftIndex().value() << "]";
            leftInstance = sBuff.str();
          }

          if (inc.rightIndex().value() > -1 && real_sys.isArray(rightInstance)) {
            std::stringstream sBuff;
            sBuff << rightInstance << "[" << inc.rightIndex().value() << "]";
            rightInstance = sBuff.str();
          }

          factory.setReferenceSlot(leftInstance, leftReference, rightInstance);
        }
      }

      template < typename GUM_SCALAR >
      INLINE bool O3SystemFactory< GUM_SCALAR >::checkSystem__(O3System& sys) {
        if (checkInstance__(sys) && checkAssignments__(sys)
            && checkIncrements__(sys)) {
          return true;
        }

        return false;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3SystemFactory< GUM_SCALAR >::checkInstance__(O3System& sys) {
        for (auto& i: sys.instances()) {
          if (!solver__->resolveClass(i.type())) { return false; }

          const auto& type = prm__->getClass(i.type().label());
          if (type.parameters().size() > 0) {
            if (!checkParameters__(type, i)) { return false; }
          }

          if (nameMap__.exists(i.name().label())) {
            O3PRM_SYSTEM_DUPLICATE_INSTANCE(i, *errors__);
            return false;
          }

          nameMap__.insert(i.name().label(), &i);
        }

        return true;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3SystemFactory< GUM_SCALAR >::checkParameters__(
         const PRMClass< GUM_SCALAR >& type, const O3Instance& inst) {
        for (const auto& param: inst.parameters()) {
          if (!type.exists(param.name().label())) {
            O3PRM_SYSTEM_PARAMETER_NOT_FOUND(param, *errors__);
            return false;
          }

          if (!PRMClassElement< GUM_SCALAR >::isParameter(
                 type.get(param.name().label()))) {
            O3PRM_SYSTEM_NOT_A_PARAMETER(param, *errors__);
            return false;
          }

          const auto& type_param =
             static_cast< const PRMParameter< GUM_SCALAR >& >(
                type.get(param.name().label()));

          switch (type_param.valueType()) {
            case PRMParameter< GUM_SCALAR >::ParameterType::INT: {
              if (!param.isInteger()) {
                O3PRM_SYSTEM_PARAMETER_NOT_INT(param, *errors__);
                return false;
              }
              break;
            }

            case PRMParameter< GUM_SCALAR >::ParameterType::REAL: {
              if (param.isInteger()) {
                O3PRM_SYSTEM_PARAMETER_NOT_FLOAT(param, *errors__);
                return false;
              }
              break;
            }

            default: {
              GUM_ERROR(FatalError, "unknown parameter type");
            }
          }
        }
        return true;
      }

      template < typename GUM_SCALAR >
      INLINE bool
         O3SystemFactory< GUM_SCALAR >::checkAssignments__(O3System& sys) {
        for (auto& ass: sys.assignments()) {
          // if ( ass.leftInstance().label() == ass.leftReference().label() ) {
          //  O3PRM_SYSTEM_INVALID_LEFT_VALUE( ass.leftInstance(), *errors__ );
          //  return false;
          //}

          if (!nameMap__.exists(ass.leftInstance().label())) {
            O3PRM_SYSTEM_INSTANCE_NOT_FOUND(ass.leftInstance(), *errors__);
            return false;
          }

          auto        i = nameMap__[ass.leftInstance().label()];
          const auto& type = prm__->getClass(i->type().label());
          const auto& ref = ass.leftReference().label();

          if (!(type.exists(ass.leftReference().label())
                && PRMClassElement< GUM_SCALAR >::isReferenceSlot(
                   type.get(ref)))) {
            O3PRM_SYSTEM_REFERENCE_NOT_FOUND(
               ass.leftReference(), type.name(), *errors__);
            return false;
          }

          const auto& real_ref =
             static_cast< const PRMReferenceSlot< GUM_SCALAR >& >(type.get(ref));

          if (!nameMap__.exists(ass.rightInstance().label())) {
            O3PRM_SYSTEM_INSTANCE_NOT_FOUND(ass.rightInstance(), *errors__);
            return false;
          }

          if (real_ref.isArray()
              && nameMap__[ass.rightInstance().label()]->size().value() == 0) {
            O3PRM_SYSTEM_NOT_AN_ARRAY(ass.rightInstance(), *errors__);
            return false;
          }

          if ((!real_ref.isArray())
              && nameMap__[ass.rightInstance().label()]->size().value() > 0
              && ass.rightIndex().value() == -1) {
            O3PRM_SYSTEM_NOT_AN_ARRAY(ass.leftReference(), *errors__);
            return false;
          }
        }
        return true;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3SystemFactory< GUM_SCALAR >::checkIncrements__(O3System& sys) {
        for (auto& inc: sys.increments()) {
          // if ( inc.leftInstance().label() == inc.leftReference().label() ) {
          //  O3PRM_SYSTEM_INVALID_LEFT_VALUE( inc.leftInstance(), *errors__ );
          //  return false;
          //}

          if (!nameMap__.exists(inc.leftInstance().label())) {
            O3PRM_SYSTEM_INSTANCE_NOT_FOUND(inc.leftInstance(), *errors__);
            return false;
          }

          auto        i = nameMap__[inc.leftInstance().label()];
          const auto& type = prm__->getClass(i->type().label());
          const auto& ref = inc.leftReference().label();

          if (!(type.exists(inc.leftReference().label())
                && PRMClassElement< GUM_SCALAR >::isReferenceSlot(
                   type.get(ref)))) {
            O3PRM_SYSTEM_REFERENCE_NOT_FOUND(
               inc.leftReference(), type.name(), *errors__);
            return false;
          }

          const auto& real_ref =
             static_cast< const PRMReferenceSlot< GUM_SCALAR >& >(type.get(ref));

          if (!real_ref.isArray()) {
            O3PRM_SYSTEM_NOT_AN_ARRAY(inc.leftReference(), *errors__);
            return false;
          }
        }

        return true;
      }
    }   // namespace o3prm
  }     // namespace prm
}   // namespace gum
