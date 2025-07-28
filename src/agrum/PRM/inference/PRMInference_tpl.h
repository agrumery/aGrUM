/****************************************************************************
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
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Inline implementation of PRMInference.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 *
 */
#include <agrum/PRM/inference/PRMInference.h>

namespace gum {
  namespace prm {

    template < typename GUM_SCALAR >
    void PRMInference< GUM_SCALAR >::clearEvidence() {
      for (const auto& elt: _evidences_) {
        for (const auto& elt2: *elt.second)
          delete elt2.second;

        delete elt.second;
      }

      _evidences_.clear();
    }

    template < typename GUM_SCALAR >
    PRMInference< GUM_SCALAR >::PRMInference(const PRMInference< GUM_SCALAR >& source) :
        prm_(source.prm_), sys_(source.sys_) {
      GUM_CONS_CPY(PRMInference);

      for (const auto& elt: source._evidences_) {
        _evidences_.insert(elt.first, new PRMInference< GUM_SCALAR >::EMap());

        for (const auto& elt2: *elt.second) {
          Tensor< GUM_SCALAR >* e = new Tensor< GUM_SCALAR >();
          e->add(*(elt2.second->variablesSequence().front()));
          Instantiation i(*e);

          for (i.setFirst(); !i.end(); i.inc())
            e->set(i, elt2.second->get(i));

          _evidences_[elt.first]->insert(elt2.first, e);
        }
      }
    }

    template < typename GUM_SCALAR >
    PRMInference< GUM_SCALAR >&
        PRMInference< GUM_SCALAR >::operator=(const PRMInference< GUM_SCALAR >& source) {
      clearEvidence();
      prm_ = source.prm_;
      sys_ = source.sys_;

      for (const auto& elt: source._evidences_) {
        _evidences_.insert(elt.first, new PRMInference< GUM_SCALAR >::EMap());

        for (const auto& elt2: *elt.second) {
          Tensor< GUM_SCALAR >* e = new Tensor< GUM_SCALAR >();
          e->add(*(elt2.second->variablesSequence().front()));
          Instantiation i(*e);

          for (i.setFirst(); !i.end(); i.inc()) {
            e->set(i, elt2.second->get(i));
          }

          _evidences_[elt.first]->insert(elt2.first, e);
        }
      }

      return *this;
    }

    template < typename GUM_SCALAR >
    typename PRMInference< GUM_SCALAR >::EMap&
        PRMInference< GUM_SCALAR >::_EMap_(const PRMInstance< GUM_SCALAR >* i) {
      if (_evidences_.exists(i)) {
        return *(_evidences_[i]);
      } else {
        _evidences_.insert(i, new PRMInference< GUM_SCALAR >::EMap());
        return *(_evidences_[i]);
      }
    }

    template < typename GUM_SCALAR >
    void PRMInference< GUM_SCALAR >::addEvidence(const Chain&                chain,
                                                 const Tensor< GUM_SCALAR >& p) {
      if (chain.first->exists(chain.second->id())) {
        if ((p.nbrDim() != 1) || (!p.contains(chain.second->type().variable())))
          GUM_ERROR(OperationNotAllowed, "illegal evidence for the given PRMAttribute.")

        Tensor< GUM_SCALAR >* e = new Tensor< GUM_SCALAR >();
        e->add(chain.second->type().variable());
        Instantiation i(*e);

        for (i.setFirst(); !i.end(); i.inc())
          e->set(i, p.get(i));

        PRMInference< GUM_SCALAR >::EMap& emap = _EMap_(chain.first);

        if (emap.exists(chain.second->id())) {
          delete emap[chain.second->id()];
          emap[chain.second->id()] = e;
        } else {
          emap.insert(chain.second->id(), e);
        }

        evidenceAdded_(chain);
      } else {
        GUM_ERROR(NotFound,
                  "the given PRMAttribute does not belong to this "
                  "Instance<GUM_SCALAR>.");
      }
    }

    template < typename GUM_SCALAR >
    INLINE PRMInference< GUM_SCALAR >::PRMInference(const PRM< GUM_SCALAR >&       prm,
                                                    const PRMSystem< GUM_SCALAR >& system) :
        prm_(&prm), sys_(&system) {
      GUM_CONSTRUCTOR(PRMInference);
    }

    template < typename GUM_SCALAR >
    INLINE PRMInference< GUM_SCALAR >::~PRMInference() {
      GUM_DESTRUCTOR(PRMInference);
      clearEvidence();
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMInference< GUM_SCALAR >::EMap&
        PRMInference< GUM_SCALAR >::evidence(const PRMInstance< GUM_SCALAR >& i) {
      try {
        return *(_evidences_[&i]);
      } catch (NotFound const&) { GUM_ERROR(NotFound, "this instance has no evidence.") }
    }

    template < typename GUM_SCALAR >
    INLINE const typename PRMInference< GUM_SCALAR >::EMap&
        PRMInference< GUM_SCALAR >::evidence(const PRMInstance< GUM_SCALAR >& i) const {
      try {
        return *(_evidences_[&i]);
      } catch (NotFound const&) { GUM_ERROR(NotFound, "this instance has no evidence.") }
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMInference< GUM_SCALAR >::EMap&
        PRMInference< GUM_SCALAR >::evidence(const PRMInstance< GUM_SCALAR >* i) {
      try {
        return *(_evidences_[i]);
      } catch (NotFound const&) { GUM_ERROR(NotFound, "this instance has no evidence.") }
    }

    template < typename GUM_SCALAR >
    INLINE const typename PRMInference< GUM_SCALAR >::EMap&
        PRMInference< GUM_SCALAR >::evidence(const PRMInstance< GUM_SCALAR >* i) const {
      try {
        return *(_evidences_[i]);
      } catch (NotFound const&) { GUM_ERROR(NotFound, "this instance has no evidence.") }
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMInference< GUM_SCALAR >::hasEvidence(const PRMInstance< GUM_SCALAR >& i) const {
      return _evidences_.exists(&i);
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMInference< GUM_SCALAR >::hasEvidence(const PRMInstance< GUM_SCALAR >* i) const {
      return _evidences_.exists(i);
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMInference< GUM_SCALAR >::hasEvidence(const Chain& chain) const {
      return (hasEvidence(chain.first)) ? evidence(chain.first).exists(chain.second->id()) : false;
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMInference< GUM_SCALAR >::hasEvidence() const {
      return (_evidences_.size() != (Size)0);
    }

    template < typename GUM_SCALAR >
    INLINE void PRMInference< GUM_SCALAR >::removeEvidence(const Chain& chain) {
      try {
        if (_EMap_(chain.first).exists(chain.second->id())) {
          evidenceRemoved_(chain);
          delete _EMap_(chain.first)[chain.second->id()];
          _EMap_(chain.first).erase(chain.second->id());
        }
      } catch (NotFound const&) {
        // Ok, we are only removing
      }
    }

    template < typename GUM_SCALAR >
    INLINE void PRMInference< GUM_SCALAR >::posterior(
        const typename PRMInference< GUM_SCALAR >::Chain& chain,
        Tensor< GUM_SCALAR >&                             m) {
      if (m.nbrDim() > 0) { GUM_ERROR(OperationNotAllowed, "the given Tensor is not empty.") }

      if (hasEvidence(chain)) {
        m.add(chain.second->type().variable());
        const Tensor< GUM_SCALAR >& e = *(evidence(chain.first)[chain.second->id()]);
        Instantiation               i(m), j(e);

        for (i.setFirst(), j.setFirst(); !i.end(); i.inc(), j.inc())
          m.set(i, e.get(j));
      } else {
        if (chain.second != &(chain.first->get(chain.second->safeName()))) {
          typename PRMInference< GUM_SCALAR >::Chain good_chain
              = std::make_pair(chain.first, &(chain.first->get(chain.second->safeName())));
          m.add(good_chain.second->type().variable());
          posterior_(good_chain, m);
        } else {
          m.add(chain.second->type().variable());
          posterior_(chain, m);
        }
      }
    }

    template < typename GUM_SCALAR >
    INLINE void PRMInference< GUM_SCALAR >::joint(
        const std::vector< typename PRMInference< GUM_SCALAR >::Chain >& chains,
        Tensor< GUM_SCALAR >&                                            j) {
      if (j.nbrDim() > 0) { GUM_ERROR(OperationNotAllowed, "the given Tensor is not empty.") }

      for (auto chain = chains.begin(); chain != chains.end(); ++chain) {
        j.add(chain->second->type().variable());
      }

      joint_(chains, j);
    }

  } /* namespace prm */
} /* namespace gum */
