/**************************************************************************
*   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
*   {prenom.nom}_at_lip6.fr                                               *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/
/**
 * @file
 * @brief Inline implementation of PRMInference.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 *
 */
#include <agrum/PRM/inference/PRMInference.h>

namespace gum {
  namespace prm {

    template <typename GUM_SCALAR> void PRMInference<GUM_SCALAR>::clearEvidence() {
      for (const auto &elt : __evidences) {
        for (const auto &elt2 : *elt.second)
          delete elt2.second;

        delete elt.second;
      }

      __evidences.clear();
    }

    template <typename GUM_SCALAR>
    PRMInference<GUM_SCALAR>::PRMInference(const PRMInference<GUM_SCALAR> &source)
        : _prm(source._prm), _sys(source._sys) {
      GUM_CONS_CPY(PRMInference);

      for (const auto &elt : source.__evidences) {
        __evidences.insert(elt.first, new PRMInference<GUM_SCALAR>::EMap());

        for (const auto &elt2 : *elt.second) {
          Potential<GUM_SCALAR> *e = new Potential<GUM_SCALAR>();
          e->add(*(elt2.second->variablesSequence().front()));
          Instantiation i(*e);

          for (i.setFirst(); not i.end(); i.inc())
            e->set(i, elt2.second->get(i));

          __evidences[elt.first]->insert(elt2.first, e);
        }
      }
    }

    template <typename GUM_SCALAR>
    PRMInference<GUM_SCALAR> &PRMInference<GUM_SCALAR>::
    operator=(const PRMInference<GUM_SCALAR> &source) {
      clearEvidence();
      _prm = source._prm;
      _sys = source._sys;

      for (const auto &elt : source.__evidences) {
        __evidences.insert(elt.first, new PRMInference<GUM_SCALAR>::EMap());

        for (const auto &elt2 : *elt.second) {
          Potential<GUM_SCALAR> *e = new Potential<GUM_SCALAR>();
          e->add(*(elt2.second->variablesSequence().front()));
          Instantiation i(*e);

          for (i.setFirst(); not i.end(); i.inc()) {
            e->set(i, elt2.second->get(i));
          }

          __evidences[elt.first]->insert(elt2.first, e);
        }
      }

      return *this;
    }

    template <typename GUM_SCALAR>
    typename PRMInference<GUM_SCALAR>::EMap &
    PRMInference<GUM_SCALAR>::__EMap(const Instance<GUM_SCALAR> *i) {
      if (__evidences.exists(i)) {
        return *(__evidences[i]);
      } else {
        __evidences.insert(i, new PRMInference<GUM_SCALAR>::EMap());
        return *(__evidences[i]);
      }
    }

    template <typename GUM_SCALAR>
    void PRMInference<GUM_SCALAR>::addEvidence(const Chain &chain,
                                               const Potential<GUM_SCALAR> &p) {
      if (chain.first->exists(chain.second->id())) {
        if ((p.nbrDim() != 1) or (not p.contains(chain.second->type().variable())))
          GUM_ERROR(OperationNotAllowed,
                    "illegal evidence for the given Attribute.");

        Potential<GUM_SCALAR> *e = new Potential<GUM_SCALAR>();
        e->add(chain.second->type().variable());
        Instantiation i(*e);

        for (i.setFirst(); not i.end(); i.inc())
          e->set(i, p.get(i));

        PRMInference<GUM_SCALAR>::EMap &emap = __EMap(chain.first);

        if (emap.exists(chain.second->id())) {
          delete emap[chain.second->id()];
          emap[chain.second->id()] = e;
        } else {
          emap.insert(chain.second->id(), e);
        }

        _evidenceAdded(chain);
      } else {
        GUM_ERROR(
            NotFound,
            "the given Attribute does not belong to this Instance<GUM_SCALAR>.");
      }
    }

    template <typename GUM_SCALAR>
    INLINE PRMInference<GUM_SCALAR>::PRMInference(const PRM<GUM_SCALAR> &prm,
                                                  const System<GUM_SCALAR> &system)
        : _prm(&prm), _sys(&system) {
      GUM_CONSTRUCTOR(PRMInference);
    }

    template <typename GUM_SCALAR> INLINE PRMInference<GUM_SCALAR>::~PRMInference() {
      GUM_DESTRUCTOR(PRMInference);
      clearEvidence();
    }

    template <typename GUM_SCALAR>
    INLINE typename PRMInference<GUM_SCALAR>::EMap &
    PRMInference<GUM_SCALAR>::evidence(const Instance<GUM_SCALAR> &i) {
      try {
        return *(__evidences[&i]);
      } catch (NotFound &) {
        GUM_ERROR(NotFound, "this instance has no evidence.");
      }
    }

    template <typename GUM_SCALAR>
    INLINE const typename PRMInference<GUM_SCALAR>::EMap &
    PRMInference<GUM_SCALAR>::evidence(const Instance<GUM_SCALAR> &i) const {
      try {
        return *(__evidences[&i]);
      } catch (NotFound &) {
        GUM_ERROR(NotFound, "this instance has no evidence.");
      }
    }

    template <typename GUM_SCALAR>
    INLINE typename PRMInference<GUM_SCALAR>::EMap &
    PRMInference<GUM_SCALAR>::evidence(const Instance<GUM_SCALAR> *i) {
      try {
        return *(__evidences[i]);
      } catch (NotFound &) {
        GUM_ERROR(NotFound, "this instance has no evidence.");
      }
    }

    template <typename GUM_SCALAR>
    INLINE const typename PRMInference<GUM_SCALAR>::EMap &
    PRMInference<GUM_SCALAR>::evidence(const Instance<GUM_SCALAR> *i) const {
      try {
        return *(__evidences[i]);
      } catch (NotFound &) {
        GUM_ERROR(NotFound, "this instance has no evidence.");
      }
    }

    template <typename GUM_SCALAR>
    INLINE bool
    PRMInference<GUM_SCALAR>::hasEvidence(const Instance<GUM_SCALAR> &i) const {
      return __evidences.exists(&i);
    }

    template <typename GUM_SCALAR>
    INLINE bool
    PRMInference<GUM_SCALAR>::hasEvidence(const Instance<GUM_SCALAR> *i) const {
      return __evidences.exists(i);
    }

    template <typename GUM_SCALAR>
    INLINE bool PRMInference<GUM_SCALAR>::hasEvidence(const Chain &chain) const {
      return (hasEvidence(chain.first))
                 ? evidence(chain.first).exists(chain.second->id())
                 : false;
    }

    template <typename GUM_SCALAR>
    INLINE bool PRMInference<GUM_SCALAR>::hasEvidence() const {
      return __evidences.size();
    }

    template <typename GUM_SCALAR>
    INLINE void PRMInference<GUM_SCALAR>::removeEvidence(const Chain &chain) {
      try {
        if (__EMap(chain.first).exists(chain.second->id())) {
          _evidenceRemoved(chain);
          delete __EMap(chain.first)[chain.second->id()];
          __EMap(chain.first).erase(chain.second->id());
        }
      } catch (NotFound &) {
        // Ok, we are only removing
        GUM_CHECKPOINT;
      }
    }

    template <typename GUM_SCALAR>
    INLINE void
    PRMInference<GUM_SCALAR>::marginal(const PRMInference<GUM_SCALAR>::Chain &chain,
                                       Potential<GUM_SCALAR> &m) {
      if (m.nbrDim() > 0) {
        GUM_ERROR(OperationNotAllowed, "the given Potential is not empty.");
      }

      if (hasEvidence(chain)) {
        m.add(chain.second->type().variable());
        const Potential<GUM_SCALAR> &e =
            *(evidence(chain.first)[chain.second->id()]);
        Instantiation i(m), j(e);

        for (i.setFirst(), j.setFirst(); not i.end(); i.inc(), j.inc())
          m.set(i, e.get(j));
      } else {
        if (chain.second != &(chain.first->get(chain.second->safeName()))) {
          PRMInference<GUM_SCALAR>::Chain good_chain = std::make_pair(
              chain.first, &(chain.first->get(chain.second->safeName())));
          m.add(good_chain.second->type().variable());
          _marginal(good_chain, m);
        } else {
          m.add(chain.second->type().variable());
          _marginal(chain, m);
        }
      }
    }

    template <typename GUM_SCALAR>
    INLINE void PRMInference<GUM_SCALAR>::joint(
        const std::vector<PRMInference<GUM_SCALAR>::Chain> &chains,
        Potential<GUM_SCALAR> &j) {
      if (j.nbrDim() > 0) {
        GUM_ERROR(OperationNotAllowed, "the given Potential is not empty.");
      }

      for (auto chain = chains.begin(); chain != chains.end(); ++chain) {
        j.add(chain->second->type().variable());
      }

      _joint(chains, j);
    }

  } /* namespace prm */
} /* namespace gum */
