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

#include <agrum/BN/inference/variableElimination.h>
#include <agrum/multidim/operators/multiDimCombinationDefault.h>
namespace gum {

  template <typename GUM_SCALAR>
  INLINE VariableElimination<GUM_SCALAR>::VariableElimination(
      const IBayesNet<GUM_SCALAR> &bn)
      : BayesNetInference<GUM_SCALAR>(bn) {
    GUM_CONSTRUCTOR(VariableElimination);
  }

  template <typename GUM_SCALAR>
  INLINE VariableElimination<GUM_SCALAR>::~VariableElimination() {
    GUM_DESTRUCTOR(VariableElimination);
  }
  /*
    template<typename GUM_SCALAR> INLINE
    VariableElimination<GUM_SCALAR>::VariableElimination ( const
    VariableElimination<GUM_SCALAR>& source ) {
      GUM_CONS_CPY ( VariableElimination );
      GUM_ERROR ( FatalError, "illegal call to VariableElimination copy constructor."
    );
    }

    template<typename GUM_SCALAR> INLINE
    VariableElimination<GUM_SCALAR>&
    VariableElimination<GUM_SCALAR>::operator= ( const VariableElimination& source )
    {
      GUM_ERROR ( FatalError, "illegal call to VariableElimination copy operator." );
    }
  */
  template <typename GUM_SCALAR>
  INLINE const std::vector<NodeId> &
  VariableElimination<GUM_SCALAR>::eliminationOrder() const {
    if (__eliminationOrder.size() != 0) {
      return __eliminationOrder;
    } else {
      GUM_ERROR(OperationNotAllowed, "Elimination order uninitialized.");
    }
  }

  template <typename GUM_SCALAR>
  INLINE void VariableElimination<GUM_SCALAR>::setEliminiationOrder(
      const std::vector<NodeId> &elim) {
    __eliminationOrder = elim;
  }

  template <typename GUM_SCALAR>
  INLINE void VariableElimination<GUM_SCALAR>::makeInference() {
    __computeEliminationOrder();
    __createInitialPool();
  }

  template <typename GUM_SCALAR>
  void VariableElimination<GUM_SCALAR>::insertEvidence(
      const List<const Potential<GUM_SCALAR> *> &pot_list) {
    for (ListConstIteratorSafe<const Potential<GUM_SCALAR> *> iter =
             pot_list.cbeginSafe();
         iter != pot_list.cendSafe(); ++iter) {
      if ((*iter)->nbrDim() != 1) {
        GUM_ERROR(OperationNotAllowed,
                  "Evidence can only be giben w.r.t. one random variable");
      }

      NodeId varId = this->bn().nodeId((*iter)->variable(0));

      try {
        __evidences[varId] = *iter;
      } catch (NotFound &) {
        __evidences.insert(varId, *iter);
      }
    }

    this->_invalidatePosteriors();
  }

  template <typename GUM_SCALAR>
  INLINE void
  VariableElimination<GUM_SCALAR>::eraseEvidence(const Potential<GUM_SCALAR> *e) {
    if (e->nbrDim() != 1) {
      GUM_ERROR(OperationNotAllowed,
                "Evidence can only be giben w.r.t. one random variable");
    }

    __evidences.erase(this->bn().nodeId(e->variable(0)));
    this->_invalidatePosteriors();
  }

  template <typename GUM_SCALAR>
  INLINE void VariableElimination<GUM_SCALAR>::eraseAllEvidence() {
    __evidences.clear();
    this->_invalidatePosteriors();
  }

  template <typename GUM_SCALAR>
  void VariableElimination<GUM_SCALAR>::eliminateNodes(
      const std::vector<NodeId> &elim_order, Set<Potential<GUM_SCALAR> *> &pool,
      Set<Potential<GUM_SCALAR> *> &trash) {
    for (size_t i = 0; i < elim_order.size(); ++i) {
      __eliminateNode(elim_order[i], pool, trash);
    }
  }

  template <typename GUM_SCALAR>
  void
  VariableElimination<GUM_SCALAR>::_fillPosterior(NodeId id,
                                                  Potential<GUM_SCALAR> &marginal) {
    __computeEliminationOrder();
    __createInitialPool();
    Set<Potential<GUM_SCALAR> *> pool(__pool);

    for (size_t i = 0; i < __eliminationOrder.size(); ++i) {
      if (__eliminationOrder[i] != id) {
        __eliminateNode(__eliminationOrder[i], pool, __trash);
      }
    }

    try {
      pool.insert(const_cast<Potential<GUM_SCALAR> *>(__evidences[id]));
    } catch (NotFound &) {
      // No evidence on query
    }

    marginal.add(this->bn().variable(id));
    marginal.fill((GUM_SCALAR)1);

    for (auto pot : pool) {
      if (pot->nbrDim() > 0) {
        marginal.multiplicateBy(*pot);
      }
    }

    marginal.normalize();

    // Cleaning up the mess
    for (auto pot : __trash)
      delete pot;

    __trash.clear();
  }

  template <typename GUM_SCALAR>
  void VariableElimination<GUM_SCALAR>::__computeEliminationOrder() {
    if (__eliminationOrder.empty()) {
      NodeProperty<Size> modalities;

      for (auto node : this->bn().nodes())
        modalities.insert(node, this->bn().variable(node).domainSize());

      DefaultTriangulation triang(&(this->bn().moralGraph()), &modalities);
      __eliminationOrder = triang.eliminationOrder();
    }
  }

  template <typename GUM_SCALAR>
  void VariableElimination<GUM_SCALAR>::__createInitialPool() {
    __pool.clear();

    for (auto node : this->bn().nodes()) {
      __pool.insert(const_cast<Potential<GUM_SCALAR> *>(&(this->bn().cpt(node))));

      if (__evidences.exists(node))
        __pool.insert(const_cast<Potential<GUM_SCALAR> *>(__evidences[node]));
    }
  }
  // the function used to combine two tables
  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR> *multPotential(const Potential<GUM_SCALAR> &t1,
                                       const Potential<GUM_SCALAR> &t2) {
    return new Potential<GUM_SCALAR>(t1 * t2);
  }

  template <typename GUM_SCALAR>
  void VariableElimination<GUM_SCALAR>::__eliminateNode(
      NodeId id, Set<Potential<GUM_SCALAR> *> &pool,
      Set<Potential<GUM_SCALAR> *> &trash) {
    // THIS IS A (TEMPLATIZED) COPY OF prm::eliminateNode
    {
      const gum::DiscreteVariable *var = &(this->bn().variable(id));

      Potential<GUM_SCALAR> *pot = 0;
      Potential<GUM_SCALAR> *tmp = 0;
      Set<const DiscreteVariable *> var_set;
      var_set.insert(var);
      Set<const Potential<GUM_SCALAR> *> pots;

      for (auto pot : pool)
        if (pot->contains(*var))
          pots.insert(pot);

      if (pots.size() == 0) {
        return;
      } else if (pots.size() == 1) {
        pot = new Potential<GUM_SCALAR>(projectSum(**pots.begin(), var_set));
      } else {
        MultiDimCombinationDefault<GUM_SCALAR, Potential> Comb(multPotential);
        tmp = Comb.combine(pots);
        pot = new Potential<GUM_SCALAR>(projectSum(*tmp, var_set));
        delete tmp;
      }

      for (auto pot : pots) {
        pool.erase(const_cast<Potential<GUM_SCALAR> *>(pot));

        if (trash.exists(const_cast<Potential<GUM_SCALAR> *>(pot))) {
          trash.erase(const_cast<Potential<GUM_SCALAR> *>(pot));
          delete const_cast<Potential<GUM_SCALAR> *>(pot);
        }
      }

      pool.insert(pot);
      trash.insert(pot);
    }
  }

} /* namespace gum */
