
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @brief Detect barren nodes for inference in Bayesian networks
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_BARREN_NODES_FINDER_H
#define GUM_BARREN_NODES_FINDER_H


#include <agrum/BN/BayesNet.h>
#include <agrum/agrum.h>
#include <agrum/graphs/DAG.h>
#include <agrum/graphs/cliqueGraph.h>


namespace gum {


  /**
   * @class BarrenNodesFinder
   * @brief Detect barren nodes for inference in Bayesian networks
   * @ingroup bn_inference
   */
  class BarrenNodesFinder {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    explicit BarrenNodesFinder(const DAG* dag);

    /// copy constructor
    BarrenNodesFinder(const BarrenNodesFinder& from);

    /// move constructor
    BarrenNodesFinder(BarrenNodesFinder&& from) noexcept;

    /// destructor
    ~BarrenNodesFinder();

    /// @}


    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    BarrenNodesFinder& operator=(const BarrenNodesFinder& from);

    /// move operator
    BarrenNodesFinder& operator=(BarrenNodesFinder&& from);

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// sets a new DAG
    void setDAG(const DAG* new_dag);

    /// sets the observed nodes in the DAG
    void setEvidence(const NodeSet* observed_nodes);

    /// sets the set of target nodes we are interested in
    void setTargets(const NodeSet* target_nodes);

    /// returns the set of barren nodes
    NodeSet barrenNodes();

    /// returns the set of barren nodes in the messages sent in a junction tree
    ArcProperty< NodeSet > barrenNodes(const CliqueGraph& junction_tree);

    /// returns the set of barren potentials in messages sent in a junction tree
    template < typename GUM_SCALAR >
    ArcProperty< Set< const Potential< GUM_SCALAR >* > >
       barrenPotentials(const CliqueGraph&             junction_tree,
                        const IBayesNet< GUM_SCALAR >& bn);

    /// @}

    private:
    /// the DAG on which we compute the barren nodes
    const DAG* __dag;

    /// the set of observed nodes
    const NodeSet* __observed_nodes;

    /// the set of targeted nodes
    const NodeSet* __target_nodes;
  };


} /* namespace gum */


#ifndef GUM_NO_INLINE
#  include <agrum/BN/algorithms/barrenNodesFinder_inl.h>
#endif   // GUM_NO_INLINE


#include <agrum/BN/algorithms/barrenNodesFinder_tpl.h>


#endif /* GUM_BARREN_NODES_FINDER_H */
