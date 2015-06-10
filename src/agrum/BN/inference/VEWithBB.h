/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
 * @brief Header of the VEWithBB class.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_VE_WITH_BB_H
#define GUM_VE_WITH_BB_H

#include <vector>

#include <agrum/config.h>

#include <agrum/BN/IBayesNet.h>
#include <agrum/BN/inference/BayesNetInference.h>
#include <agrum/BN/inference/variableElimination.h>
#include <agrum/BN/inference/BayesBall.h>

namespace gum {
  /**
   * @class VEWithBB VEWithBB.h <agrum/BN/inference/VEWithBB.h>
   * @brief Implementation of the state of the art Value Elimination algorithm
   *        using the BayesBall algorithm to prune the IBayesNet.
   *
   * @ingroup bn_group
   *
   */
  template <typename GUM_SCALAR>
  class VEWithBB : public BayesNetInference<GUM_SCALAR> {
    public:
    /// @name Constructor & destructor

    /// @{

    /**
     * Default constructor.
     */
    VEWithBB(const IBayesNet<GUM_SCALAR> &bn);

    VEWithBB(const VEWithBB<GUM_SCALAR> &source) = delete;
    VEWithBB<GUM_SCALAR> &operator=(const VEWithBB<GUM_SCALAR> &source) = delete;

    /**
     * Destructor.
     */
    virtual ~VEWithBB();

    /// @}

    /// @name BayesNetInference's methods

    /// @{

    /**
     * @brief Makes the inference
     *
     * This method only computes the elimination order if needed, and proceed to
     * some basic initialization.
     *
     * If the current elimination order is smaller than the number of nodes in the
     * IBayesNet, then this method will eliminate all nodes present in the
     *elimination
     * order. Thus computing a joint probability over a set of variables.
     *
     * Use the VariableElimination::pool() method to access the set of created
     *potentials.
     */
    virtual void makeInference();

    /**
     * @brief Insert new evidence in the graph.
     * If an evidence already exists over one of the variable in pot_list, then
     * it is replaced by the new evidence in pot_list.
     */
    virtual void insertEvidence(const List<const Potential<GUM_SCALAR> *> &pot_list);

    /**
     * Remove a given evidence from the graph.
     */
    virtual void eraseEvidence(const Potential<GUM_SCALAR> *e);

    /**
     * Remove all evidence from the graph.
     */
    virtual void eraseAllEvidence();

    /// @}
    protected:
    /**
     * Returns the probability of the variable.
     *
     * @param id The variable's id.
     * @param posterior the potential to fill
     * @throw ElementNotFound Raised if no variable matches id.
     */
    virtual void _fillPosterior(NodeId id, Potential<GUM_SCALAR> &posterior);

    private:
    /// Mapping between nodes and their evidences.
    NodeProperty<const Potential<GUM_SCALAR> *> __hardEvidence;

    /// The VariableElimination algorithm as the inference engine
    VariableElimination<GUM_SCALAR> __ve;

    void __fillRequisiteNode(NodeId id, Set<NodeId> &requisite_nodes);
  };
  extern template class VEWithBB<float>;
  extern template class VEWithBB<double>;
} /* namespace gum */

#include <agrum/BN/inference/VEWithBB.tcc>

#endif /* GUM_VE_WITH_BB_H */
