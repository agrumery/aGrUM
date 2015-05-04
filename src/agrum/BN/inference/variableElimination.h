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
 * @brief Header of the VariableElimination class.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_VALUE_ELIMINATION_H
#define GUM_VALUE_ELIMINATION_H

#include <vector>

#include <agrum/BN/IBayesNet.h>
#include <agrum/graphs/triangulations/defaultTriangulation.h>
#include <agrum/BN/inference/BayesNetInference.h>
#include <agrum/multidim/multiDimBucket.h>
//#include <agrum/PRM/utils_prm.h>

namespace gum {

  template <typename GUM_SCALAR> class VEWithBB;
  /**
   * @class VariableElimination variableElimination.h
   *<agrum/BN/inference/variableElimination.h>
   * @brief Implementation of the state of the art Value Elimination algorithm.
   *
   * The elimination order used by this algorithm is computed with the
   * gum::DefaultTriangulation class. However the order is computed only
   * when the makeInference() or marginal() method is called, so you can
   * give an other order without any unnecessary computation.
   *
   * @ingroup bn_group
   *
   */
  template <typename GUM_SCALAR>
  class VariableElimination : public BayesNetInference<GUM_SCALAR> {
    public:
    friend class VEWithBB<GUM_SCALAR>;

    /// @name Constructor & destructor

    /// @{

    /**
     * Default constructor.
     */
    VariableElimination(const IBayesNet<GUM_SCALAR> &bn);

    VariableElimination(const VariableElimination<GUM_SCALAR> &source) = delete;
    VariableElimination &
    operator=(const VariableElimination<GUM_SCALAR> &source) = delete;

    /**
     * Destructor.
     */
    virtual ~VariableElimination();

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

    /// @name Specific VariableElimination's methods

    /// @{

    /**
     * Returns a constant reference over the sequence used as order elimination.
     * @throw OperationNotAllowed Raised if the elimination order has not been
     *        computed yet.
     */
    const std::vector<NodeId> &eliminationOrder() const;

    /**
     * @brief Getter on the elimination order used.
     *
     * This method checks that all nodes present in the BN are in elim.
     */
    void setEliminiationOrder(const std::vector<NodeId> &elim);

    /**
     * @brief Eliminate nodes in elim_order using pool as initial potential pool.
     * potentials.
     * @param elim_order An elimination order, which must be a subset of nodes in
     *                   the class IBayesNet.
     * @param pool Set of Potential used as initial pool for the elimination.
     *             Results are stored in it also.
     * @param trash The Set of Potential to delete after use of those in pool.
     */
    void eliminateNodes(const std::vector<NodeId> &elim_order,
                        Set<Potential<GUM_SCALAR> *> &pool,
                        Set<Potential<GUM_SCALAR> *> &trash);

    /// @}

    protected:
    /**
     * Returns the probability of the variable.
     *
     * @param id The variable's id.
     * @param marginal the potential to fill
     * @throw ElementNotFound Raised if no variable matches id.
     */
    virtual void _fillPosterior(NodeId id, Potential<GUM_SCALAR> &marginal);

    private:
    /// Mapping between nodes and their evidences.
    NodeProperty<const Potential<GUM_SCALAR> *> __evidences;

    /// The elimination order used by this algorithm.
    std::vector<NodeId> __eliminationOrder;

    /// The initial pool of potentials.
    Set<Potential<GUM_SCALAR> *> __pool;

    /// Garbage collector over the buckets created during inference
    Set<Potential<GUM_SCALAR> *> __trash;

    /// Compute the elimination order if __eliminationOrder is empty.
    void __computeEliminationOrder();

    /// Fills the pool with all the potentials in __bayesNet.
    void __createInitialPool();

    /// Fills the bucket with all the potentials in pool containing id and
    /// insert it in pool as a Potential after removing all the potentials
    /// already in it. If you don't understand, read the code...
    void __eliminateNode(NodeId id, Set<Potential<GUM_SCALAR> *> &pool,
                         Set<Potential<GUM_SCALAR> *> &trash);
  };

  extern template class VariableElimination<float>;
  extern template class VariableElimination<double>;
} /* namespace gum */

#include <agrum/BN/inference/variableElimination.tcc>

#endif /* GUM_VALUE_ELIMINATION_H */
