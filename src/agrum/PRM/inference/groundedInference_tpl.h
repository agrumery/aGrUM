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
 * @brief Inline implementation of GroundedInference.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(@LIP6)
 *
 */
#include <agrum/PRM/inference/groundedInference.h>

namespace gum {
  namespace prm {

    template < typename GUM_SCALAR >
    GroundedInference< GUM_SCALAR >::~GroundedInference() {
      GUM_DESTRUCTOR(GroundedInference);

      if (inf__ != nullptr) delete inf__;

      if (!obs__.empty())
        for (const auto pot: obs__)
          // We used const ptrs only because of
          // MarginalTargetedInference::addEvidence()
          // requires it
          delete const_cast< Potential< GUM_SCALAR >* >(pot);
    }

    template < typename GUM_SCALAR >
    void GroundedInference< GUM_SCALAR >::evidenceAdded_(
       const typename PRMInference< GUM_SCALAR >::Chain& chain) {
      Potential< GUM_SCALAR >* bn_obs = new Potential< GUM_SCALAR >();
      // Retrieving the BN's variable
      std::stringstream var_name;
      var_name << chain.first->name() << "." << chain.second->safeName();
      bn_obs->add(inf__->BN().variableFromName(var_name.str()));
      // Retrievin the PRM<GUM_SCALAR>'s evidence and copying it in bn_obs
      const Potential< GUM_SCALAR >* prm_obs
         = this->evidence(chain.first)[chain.second->id()];
      Instantiation i(*bn_obs), j(*prm_obs);

      for (i.setFirst(), j.setFirst(); !i.end(); i.inc(), j.inc()) {
        bn_obs->set(i, prm_obs->get(j));
      }

      obs__.insert(bn_obs);
    }

    template < typename GUM_SCALAR >
    void GroundedInference< GUM_SCALAR >::evidenceRemoved_(
       const typename PRMInference< GUM_SCALAR >::Chain& chain) {
      std::stringstream var_name;
      var_name << chain.first->name() << "." << chain.second->safeName();
      const DiscreteVariable& var = inf__->BN().variableFromName(var_name.str());

      for (auto iter = obs__.beginSafe(); iter != obs__.endSafe();
           ++iter) {   // safe iterator needed here
        if ((**iter).contains(var)) {
          inf__->eraseEvidence(var_name.str());
          const Potential< GUM_SCALAR >* e = *iter;
          obs__.erase(iter);
          delete e;
          break;
        }
      }
    }

    template < typename GUM_SCALAR >
    INLINE GroundedInference< GUM_SCALAR >::GroundedInference(
       const PRM< GUM_SCALAR >&       prm,
       const PRMSystem< GUM_SCALAR >& system) :
        PRMInference< GUM_SCALAR >(prm, system),
        inf__(0) {
      GUM_CONSTRUCTOR(GroundedInference);
    }

    template < typename GUM_SCALAR >
    INLINE GroundedInference< GUM_SCALAR >::GroundedInference(
       const GroundedInference< GUM_SCALAR >& source) :
        PRMInference< GUM_SCALAR >(source),
        inf__(0) {
      GUM_CONS_CPY(GroundedInference);
      GUM_ERROR(FatalError, "illegal to copy constructor")
    }

    template < typename GUM_SCALAR >
    INLINE GroundedInference< GUM_SCALAR >&
       GroundedInference< GUM_SCALAR >::operator=(
          const GroundedInference< GUM_SCALAR >& source) {
      GUM_ERROR(FatalError, "illegal call to copy operator")
    }

    template < typename GUM_SCALAR >
    INLINE MarginalTargetedInference< GUM_SCALAR >&
           GroundedInference< GUM_SCALAR >::getBNInference() {
      if (inf__ != 0) {
        return *inf__;
      } else {
        GUM_ERROR(NotFound, "the inference engine is not yet defined")
      }
    }

    template < typename GUM_SCALAR >
    INLINE void GroundedInference< GUM_SCALAR >::setBNInference(
       MarginalTargetedInference< GUM_SCALAR >* bn_inf) {
      if (inf__ != 0) { delete inf__; }

      inf__ = bn_inf;
    }

    template < typename GUM_SCALAR >
    INLINE void GroundedInference< GUM_SCALAR >::posterior_(
       const typename PRMInference< GUM_SCALAR >::Chain& chain,
       Potential< GUM_SCALAR >&                          m) {
      if (inf__ == 0) {
        GUM_ERROR(OperationNotAllowed, "no inference engine defined")
      }

      std::stringstream sBuff;

      if (!obs__.empty()) {
        for (auto e: obs__) {
          try {
            inf__->addEvidence(*e);
          } catch (InvalidArgument&) { inf__->chgEvidence(*e); }
        }
      }

      sBuff << chain.first->name() << "." << chain.second->safeName();
      m = inf__->posterior(inf__->BN().idFromName(sBuff.str()));
    }

    template < typename GUM_SCALAR >
    INLINE void GroundedInference< GUM_SCALAR >::joint_(
       const std::vector< typename PRMInference< GUM_SCALAR >::Chain >& queries,
       Potential< GUM_SCALAR >&                                         j) {
      GUM_ERROR(FatalError, "not yet implemented")
    }

    template < typename GUM_SCALAR >
    INLINE std::string GroundedInference< GUM_SCALAR >::name() const {
      return "grounded inference";
    }

  } /* namespace prm */
} /* namespace gum */
