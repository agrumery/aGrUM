/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES et Pierre-Henri WUILLEMIN   *
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
 * @brief Implementation of lazy propagation for inference Bayesian Networks.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#include <agrum/BN/inference/VEWithBB.h>

namespace gum {

  template <typename GUM_SCALAR>
  INLINE VEWithBB<GUM_SCALAR>::VEWithBB(const IBayesNet<GUM_SCALAR> &bn)
      : BayesNetInference<GUM_SCALAR>(bn), __ve(bn) {
    GUM_CONSTRUCTOR(VEWithBB);
    __ve.makeInference();
  }

  template <typename GUM_SCALAR> INLINE VEWithBB<GUM_SCALAR>::~VEWithBB() {
    GUM_DESTRUCTOR(VEWithBB);
  }
  /*
    template<typename GUM_SCALAR> INLINE
    VEWithBB<GUM_SCALAR>::VEWithBB( const VEWithBB<GUM_SCALAR>& source ) :
    BayesNetInference<GUM_SCALAR>(source) {
      GUM_CONS_CPY( VEWithBB );
      GUM_ERROR( FatalError, "illegal call to VEWithBB copy constructor." );
    }

    template<typename GUM_SCALAR> INLINE
    VEWithBB<GUM_SCALAR>&
    VEWithBB<GUM_SCALAR>::operator=( const VEWithBB& source ) {
      GUM_ERROR( FatalError, "illegal call to VEWithBB copy operator." );
    }*/

  template <typename GUM_SCALAR> INLINE void VEWithBB<GUM_SCALAR>::makeInference() {}

  template <typename GUM_SCALAR>
  void VEWithBB<GUM_SCALAR>::insertEvidence(
      const List<const Potential<GUM_SCALAR> *> &pot_list) {
    for (const auto &pot : pot_list) {
      if (pot->nbrDim() != 1) {
        GUM_ERROR(OperationNotAllowed,
                  "Evidence can only be giben w.r.t. one random variable");
      }

      NodeId varId = this->bn().nodeId(*(pot->variablesSequence().atPos(0)));
      size_t count = 0;
      Instantiation i(*pot);

      for (i.setFirst(); not i.end(); i.inc()) {
        if (pot->get(i) == (GUM_SCALAR)1) {
          ++count;
        }

        if (count > 2) {
          break;
        }
      }

      if (count == 1) {
        __hardEvidence.insert(varId, pot);
      }
    }

    __ve.insertEvidence(pot_list);
    this->_invalidatePosteriors();
  }

  template <typename GUM_SCALAR>
  INLINE void VEWithBB<GUM_SCALAR>::eraseEvidence(const Potential<GUM_SCALAR> *e) {
    if (e->nbrDim() != 1) {
      GUM_ERROR(OperationNotAllowed,
                "Evidence can only be giben w.r.t. one random variable");
    }

    __hardEvidence.erase((this->bn().nodeId(*(e->variablesSequence().atPos(0)))));
    __ve.eraseEvidence(e);
    this->_invalidatePosteriors();
  }

  template <typename GUM_SCALAR>
  INLINE void VEWithBB<GUM_SCALAR>::eraseAllEvidence() {
    __hardEvidence.clear();
    __ve.eraseAllEvidence();
    this->_invalidatePosteriors();
  }

  template <typename GUM_SCALAR>
  INLINE void
  VEWithBB<GUM_SCALAR>::__fillRequisiteNode(NodeId id,
                                            Set<NodeId> &requisite_nodes) {
    Set<NodeId> query;
    query.insert(id);
    Set<NodeId> hardEvidence;
    Set<NodeId> softEvidence;

    for (const auto &elt : __hardEvidence)
      hardEvidence.insert(elt.first);

    BayesBall bb;
    bb.requisiteNodes(this->bn().dag(), query, hardEvidence, softEvidence, requisite_nodes);
  }

  template <typename GUM_SCALAR>
  void VEWithBB<GUM_SCALAR>::_fillPosterior(NodeId id,
                                            Potential<GUM_SCALAR> &posterior) {
    Set<NodeId> requisite_nodes;
    __fillRequisiteNode(id, requisite_nodes);
    Set<Potential<GUM_SCALAR> *> pool;
    Set<NodeId> elim_set;

    for (const auto node : requisite_nodes) {
      pool.insert(const_cast<Potential<GUM_SCALAR> *>(&(this->bn().cpt(node))));
      elim_set.insert(node);

      for (const auto parent : this->bn().dag().parents(node))
        if (__hardEvidence.exists(parent))
          elim_set.insert(parent);
    }

    elim_set.erase(id);

    std::vector<NodeId> elim_order;

    for (size_t idx = 0; idx < __ve.eliminationOrder().size(); ++idx) {
      if (elim_set.contains(__ve.eliminationOrder()[idx])) {
        elim_order.push_back(__ve.eliminationOrder()[idx]);
      }
    }

    Set<Potential<GUM_SCALAR> *> trash;
    __ve.eliminateNodes(elim_order, pool, trash);
    posterior.add(this->bn().variable(id));

    try {
      pool.insert(const_cast<Potential<GUM_SCALAR> *>(__ve.__evidences[id]));
    } catch (NotFound &) {
      // No evidence on query
    }

    MultiDimBucket<GUM_SCALAR> bucket;

    for (const auto pot : pool)
      bucket.add(*pot);

    bucket.add(this->bn().variable(id));
    Instantiation i(posterior);
    Instantiation j(bucket);

    for (i.setFirst(), j.setFirst(); not i.end(); i.inc(), j.inc()) {
      posterior.set(i, bucket.get(j));
    }

    posterior.normalize();

    for (const auto pot : trash)
      delete pot;
  }

} /* namespace gum */
