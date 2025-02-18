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
 * @brief Detect barren nodes for inference in Bayesian networks
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_BARREN_NODES_FINDER_H
#define GUM_BARREN_NODES_FINDER_H


#include <agrum/agrum.h>

#include <agrum/base/graphs/cliqueGraph.h>
#include <agrum/BN/BayesNet.h>

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

    /// returns the set of barren tensors in messages sent in a junction tree
    template < typename GUM_SCALAR >
    ArcProperty< Set< const Tensor< GUM_SCALAR >* > >
        barrenTensors(const CliqueGraph& junction_tree, const IBayesNet< GUM_SCALAR >& bn);

    /// @}

    private:
    /// the DAG on which we compute the barren nodes
    const DAG* _dag_;

    /// the set of observed nodes
    const NodeSet* _observed_nodes_;

    /// the set of targeted nodes
    const NodeSet* _target_nodes_;
  };


} /* namespace gum */


#ifndef GUM_NO_INLINE
#  include <agrum/BN/algorithms/barrenNodesFinder_inl.h>
#endif   // GUM_NO_INLINE


#include <agrum/BN/algorithms/barrenNodesFinder_tpl.h>


#endif /* GUM_BARREN_NODES_FINDER_H */
