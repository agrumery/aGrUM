/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Inline implementation of GroundedInference.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 *
 */
#include <agrum/PRM/inference/groundedInference.h>

namespace gum {
  namespace prm {

    template < typename GUM_SCALAR >
    GroundedInference< GUM_SCALAR >::~GroundedInference() {
      GUM_DESTRUCTOR(GroundedInference);

      if (_inf_ != nullptr) delete _inf_;

      if (!_obs_.empty())
        for (const auto pot: _obs_)
          // We used const ptrs only because of
          // MarginalTargetedInference::addEvidence()
          // requires it
          delete const_cast< Tensor< GUM_SCALAR >* >(pot);
    }

    template < typename GUM_SCALAR >
    void GroundedInference< GUM_SCALAR >::evidenceAdded_(
        const typename PRMInference< GUM_SCALAR >::Chain& chain) {
      Tensor< GUM_SCALAR >* bn_obs = new Tensor< GUM_SCALAR >();
      // Retrieving the BN's variable
      std::stringstream var_name;
      var_name << chain.first->name() << "." << chain.second->safeName();
      bn_obs->add(_inf_->BN().variableFromName(var_name.str()));
      // Retrievin the PRM<GUM_SCALAR>'s evidence and copying it in bn_obs
      const Tensor< GUM_SCALAR >* prm_obs = this->evidence(chain.first)[chain.second->id()];
      Instantiation                  i(*bn_obs), j(*prm_obs);

      for (i.setFirst(), j.setFirst(); !i.end(); i.inc(), j.inc()) {
        bn_obs->set(i, prm_obs->get(j));
      }

      _obs_.insert(bn_obs);
    }

    template < typename GUM_SCALAR >
    void GroundedInference< GUM_SCALAR >::evidenceRemoved_(
        const typename PRMInference< GUM_SCALAR >::Chain& chain) {
      std::stringstream var_name;
      var_name << chain.first->name() << "." << chain.second->safeName();
      const DiscreteVariable& var = _inf_->BN().variableFromName(var_name.str());

      for (auto iter = _obs_.beginSafe(); iter != _obs_.endSafe();
           ++iter) {   // safe iterator needed here
        if ((**iter).contains(var)) {
          _inf_->eraseEvidence(var_name.str());
          const Tensor< GUM_SCALAR >* e = *iter;
          _obs_.erase(iter);
          delete e;
          break;
        }
      }
    }

    template < typename GUM_SCALAR >
    INLINE
        GroundedInference< GUM_SCALAR >::GroundedInference(const PRM< GUM_SCALAR >&       prm,
                                                           const PRMSystem< GUM_SCALAR >& system) :
        PRMInference< GUM_SCALAR >(prm, system), _inf_(0) {
      GUM_CONSTRUCTOR(GroundedInference);
    }

    template < typename GUM_SCALAR >
    INLINE GroundedInference< GUM_SCALAR >::GroundedInference(
        const GroundedInference< GUM_SCALAR >& source) :
        PRMInference< GUM_SCALAR >(source), _inf_(0) {
      GUM_CONS_CPY(GroundedInference);
      GUM_ERROR(FatalError, "illegal to copy constructor")
    }

    template < typename GUM_SCALAR >
    INLINE GroundedInference< GUM_SCALAR >&
        GroundedInference< GUM_SCALAR >::operator=(const GroundedInference< GUM_SCALAR >& source) {
      GUM_ERROR(FatalError, "illegal call to copy operator")
    }

    template < typename GUM_SCALAR >
    INLINE MarginalTargetedInference< GUM_SCALAR >&
           GroundedInference< GUM_SCALAR >::getBNInference() {
      if (_inf_ != 0) {
        return *_inf_;
      } else {
        GUM_ERROR(NotFound, "the inference engine is not yet defined")
      }
    }

    template < typename GUM_SCALAR >
    INLINE void GroundedInference< GUM_SCALAR >::setBNInference(
        MarginalTargetedInference< GUM_SCALAR >* bn_inf) {
      if (_inf_ != 0) { delete _inf_; }

      _inf_ = bn_inf;
    }

    template < typename GUM_SCALAR >
    INLINE void GroundedInference< GUM_SCALAR >::posterior_(
        const typename PRMInference< GUM_SCALAR >::Chain& chain,
        Tensor< GUM_SCALAR >&                          m) {
      if (_inf_ == 0) { GUM_ERROR(OperationNotAllowed, "no inference engine defined") }

      std::stringstream sBuff;

      if (!_obs_.empty()) {
        for (auto e: _obs_) {
          try {
            _inf_->addEvidence(*e);
          } catch (InvalidArgument const&) { _inf_->chgEvidence(*e); }
        }
      }

      sBuff << chain.first->name() << "." << chain.second->safeName();
      m = _inf_->posterior(_inf_->BN().idFromName(sBuff.str()));
    }

    template < typename GUM_SCALAR >
    INLINE void GroundedInference< GUM_SCALAR >::joint_(
        const std::vector< typename PRMInference< GUM_SCALAR >::Chain >& queries,
        Tensor< GUM_SCALAR >&                                         j) {
      GUM_ERROR(FatalError, "not yet implemented")
    }

    template < typename GUM_SCALAR >
    INLINE std::string GroundedInference< GUM_SCALAR >::name() const {
      return "grounded inference";
    }

  } /* namespace prm */
} /* namespace gum */
