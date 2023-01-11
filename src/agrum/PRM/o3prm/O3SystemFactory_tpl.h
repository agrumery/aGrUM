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
 * @brief Implementation for the O3SystemFactory class.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 * @author Lionel TORTI
 */

#include <agrum/PRM/o3prm/O3SystemFactory.h>

namespace gum {
  namespace prm {
    namespace o3prm {

      template < typename GUM_SCALAR >
      INLINE O3SystemFactory< GUM_SCALAR >::O3SystemFactory(PRM< GUM_SCALAR >&          prm,
                                                            O3PRM&                      o3_prm,
                                                            O3NameSolver< GUM_SCALAR >& solver,
                                                            ErrorsContainer&            errors) :
          _prm_(&prm),
          _o3_prm_(&o3_prm), _solver_(&solver), _errors_(&errors) {
        GUM_CONSTRUCTOR(O3SystemFactory);
      }

      template < typename GUM_SCALAR >
      INLINE
         O3SystemFactory< GUM_SCALAR >::O3SystemFactory(const O3SystemFactory< GUM_SCALAR >& src) :
          _prm_(src._prm_),
          _o3_prm_(src._o3_prm_), _solver_(src._solver_), _errors_(src._errors_),
          _nameMap_(src._nameMap_) {
        GUM_CONS_CPY(O3SystemFactory);
      }

      template < typename GUM_SCALAR >
      INLINE O3SystemFactory< GUM_SCALAR >::O3SystemFactory(O3SystemFactory< GUM_SCALAR >&& src) :
          _prm_(std::move(src._prm_)), _o3_prm_(std::move(src._o3_prm_)),
          _solver_(std::move(src._solver_)), _errors_(std::move(src._errors_)),
          _nameMap_(std::move(src._nameMap_)) {
        GUM_CONS_MOV(O3SystemFactory);
      }

      template < typename GUM_SCALAR >
      INLINE O3SystemFactory< GUM_SCALAR >::~O3SystemFactory() {
        GUM_DESTRUCTOR(O3SystemFactory);
      }

      template < typename GUM_SCALAR >
      INLINE O3SystemFactory< GUM_SCALAR >&
         O3SystemFactory< GUM_SCALAR >::operator=(const O3SystemFactory< GUM_SCALAR >& src) {
        if (this == &src) { return *this; }
        _prm_    = src._prm_;
        _o3_prm_ = src._o3_prm_;
        _solver_ = src._solver_;
        _errors_ = src._errors_;
        return *this;
      }

      template < typename GUM_SCALAR >
      INLINE O3SystemFactory< GUM_SCALAR >&
         O3SystemFactory< GUM_SCALAR >::operator=(O3SystemFactory< GUM_SCALAR >&& src) {
        if (this == &src) { return *this; }
        _prm_    = std::move(src._prm_);
        _o3_prm_ = std::move(src._o3_prm_);
        _solver_ = std::move(src._solver_);
        _errors_ = std::move(src._errors_);
        return *this;
      }

      template < typename GUM_SCALAR >
      INLINE void O3SystemFactory< GUM_SCALAR >::build() {
        PRMFactory< GUM_SCALAR > factory(_prm_);

        for (auto& sys: _o3_prm_->systems()) {
          // Reseting name map for each system
          _nameMap_ = HashTable< std::string, O3Instance* >();

          if (_checkSystem_(*sys)) {
            factory.startSystem(sys->name().label());

            _addInstances_(factory, *sys);
            _addAssignments_(factory, *sys);
            _addIncrements_(factory, *sys);

            try {
              factory.endSystem();
            } catch (FatalError const&) { O3PRM_SYSTEM_INSTANTIATION_FAILED(*sys, *_errors_); }
          }
        }
      }

      template < typename GUM_SCALAR >
      INLINE void O3SystemFactory< GUM_SCALAR >::_addInstances_(PRMFactory< GUM_SCALAR >& factory,
                                                                O3System&                 sys) {
        for (auto& i: sys.instances()) {
          if (i.parameters().size() > 0) {
            auto params = HashTable< std::string, double >();
            for (auto& p: i.parameters()) {
              params.insert(p.name().label(), (double)p.value().value());
            }
            factory.addInstance(i.type().label(), i.name().label(), params);

          } else {
            if (i.size().value() > 1) {
              factory.addArray(i.type().label(), i.name().label(), i.size().value());
            } else {
              factory.addInstance(i.type().label(), i.name().label());
            }
          }
        }
      }

      template < typename GUM_SCALAR >
      INLINE void O3SystemFactory< GUM_SCALAR >::_addAssignments_(PRMFactory< GUM_SCALAR >& factory,
                                                                  O3System&                 sys) {
        const auto& real_sys = _prm_->getSystem(sys.name().label());

        for (auto& ass: sys.assignments()) {
          auto leftInstance  = ass.leftInstance().label();
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
      INLINE void O3SystemFactory< GUM_SCALAR >::_addIncrements_(PRMFactory< GUM_SCALAR >& factory,
                                                                 O3System&                 sys) {
        const auto& real_sys = _prm_->getSystem(sys.name().label());
        for (auto& inc: sys.increments()) {
          auto leftInstance  = inc.leftInstance().label();
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
      INLINE bool O3SystemFactory< GUM_SCALAR >::_checkSystem_(O3System& sys) {
        if (_checkInstance_(sys) && _checkAssignments_(sys) && _checkIncrements_(sys)) {
          return true;
        }

        return false;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3SystemFactory< GUM_SCALAR >::_checkInstance_(O3System& sys) {
        for (auto& i: sys.instances()) {
          if (!_solver_->resolveClass(i.type())) { return false; }

          const auto& type = _prm_->getClass(i.type().label());
          if (type.parameters().size() > 0) {
            if (!_checkParameters_(type, i)) { return false; }
          }

          if (_nameMap_.exists(i.name().label())) {
            O3PRM_SYSTEM_DUPLICATE_INSTANCE(i, *_errors_);
            return false;
          }

          _nameMap_.insert(i.name().label(), &i);
        }

        return true;
      }

      template < typename GUM_SCALAR >
      INLINE bool
         O3SystemFactory< GUM_SCALAR >::_checkParameters_(const PRMClass< GUM_SCALAR >& type,
                                                          const O3Instance&             inst) {
        for (const auto& param: inst.parameters()) {
          if (!type.exists(param.name().label())) {
            O3PRM_SYSTEM_PARAMETER_NOT_FOUND(param, *_errors_);
            return false;
          }

          if (!PRMClassElement< GUM_SCALAR >::isParameter(type.get(param.name().label()))) {
            O3PRM_SYSTEM_NOT_A_PARAMETER(param, *_errors_);
            return false;
          }

          const auto& type_param
             = static_cast< const PRMParameter< GUM_SCALAR >& >(type.get(param.name().label()));

          switch (type_param.valueType()) {
            case PRMParameter< GUM_SCALAR >::ParameterType::INT: {
              if (!param.isInteger()) {
                O3PRM_SYSTEM_PARAMETER_NOT_INT(param, *_errors_);
                return false;
              }
              break;
            }

            case PRMParameter< GUM_SCALAR >::ParameterType::REAL: {
              if (param.isInteger()) {
                O3PRM_SYSTEM_PARAMETER_NOT_FLOAT(param, *_errors_);
                return false;
              }
              break;
            }

            default: {
              GUM_ERROR(FatalError, "unknown parameter type")
            }
          }
        }
        return true;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3SystemFactory< GUM_SCALAR >::_checkAssignments_(O3System& sys) {
        for (auto& ass: sys.assignments()) {
          // if ( ass.leftInstance().label() == ass.leftReference().label() ) {
          //  O3PRM_SYSTEM_INVALID_LEFT_VALUE( ass.leftInstance(), * _errors_ );
          //  return false;
          //}

          if (!_nameMap_.exists(ass.leftInstance().label())) {
            O3PRM_SYSTEM_INSTANCE_NOT_FOUND(ass.leftInstance(), *_errors_);
            return false;
          }

          auto        i    = _nameMap_[ass.leftInstance().label()];
          const auto& type = _prm_->getClass(i->type().label());
          const auto& ref  = ass.leftReference().label();

          if (!(type.exists(ass.leftReference().label())
                && PRMClassElement< GUM_SCALAR >::isReferenceSlot(type.get(ref)))) {
            O3PRM_SYSTEM_REFERENCE_NOT_FOUND(ass.leftReference(), type.name(), *_errors_);
            return false;
          }

          const auto& real_ref
             = static_cast< const PRMReferenceSlot< GUM_SCALAR >& >(type.get(ref));

          if (!_nameMap_.exists(ass.rightInstance().label())) {
            O3PRM_SYSTEM_INSTANCE_NOT_FOUND(ass.rightInstance(), *_errors_);
            return false;
          }

          if (real_ref.isArray() && _nameMap_[ass.rightInstance().label()]->size().value() == 0) {
            O3PRM_SYSTEM_NOT_AN_ARRAY(ass.rightInstance(), *_errors_);
            return false;
          }

          if ((!real_ref.isArray()) && _nameMap_[ass.rightInstance().label()]->size().value() > 0
              && ass.rightIndex().value() == -1) {
            O3PRM_SYSTEM_NOT_AN_ARRAY(ass.leftReference(), *_errors_);
            return false;
          }
        }
        return true;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3SystemFactory< GUM_SCALAR >::_checkIncrements_(O3System& sys) {
        for (auto& inc: sys.increments()) {
          // if ( inc.leftInstance().label() == inc.leftReference().label() ) {
          //  O3PRM_SYSTEM_INVALID_LEFT_VALUE( inc.leftInstance(), * _errors_ );
          //  return false;
          //}

          if (!_nameMap_.exists(inc.leftInstance().label())) {
            O3PRM_SYSTEM_INSTANCE_NOT_FOUND(inc.leftInstance(), *_errors_);
            return false;
          }

          auto        i    = _nameMap_[inc.leftInstance().label()];
          const auto& type = _prm_->getClass(i->type().label());
          const auto& ref  = inc.leftReference().label();

          if (!(type.exists(inc.leftReference().label())
                && PRMClassElement< GUM_SCALAR >::isReferenceSlot(type.get(ref)))) {
            O3PRM_SYSTEM_REFERENCE_NOT_FOUND(inc.leftReference(), type.name(), *_errors_);
            return false;
          }

          const auto& real_ref
             = static_cast< const PRMReferenceSlot< GUM_SCALAR >& >(type.get(ref));

          if (!real_ref.isArray()) {
            O3PRM_SYSTEM_NOT_AN_ARRAY(inc.leftReference(), *_errors_);
            return false;
          }
        }

        return true;
      }
    }   // namespace o3prm
  }     // namespace prm
}   // namespace gum
