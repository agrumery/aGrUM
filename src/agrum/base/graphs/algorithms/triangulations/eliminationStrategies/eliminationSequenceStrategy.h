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


/** @file
 * @brief Base Class for all elimination sequence algorithms used by
 * triangulations
 *
 * This class is the interface that should be implemented by all elimination
 * sequence algorithms used by triangulation algorithms.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_ELIMINATION_SEQUENCE_STRATEGY_H
#define GUM_ELIMINATION_SEQUENCE_STRATEGY_H

#include <agrum/agrum.h>

#include <agrum/base/graphs/undiGraph.h>

namespace gum {

  /** @class EliminationSequenceStrategy
   * @brief The base class for all elimination sequence algorithms used by
   * triangulation algorithms.
   *
   * \ingroup graph_group
   *
   */
  class EliminationSequenceStrategy {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// destructor
    virtual ~EliminationSequenceStrategy();

    /** @brief creates a new elimination sequence of the same type as the
     * current object, but this sequence contains only an empty graph
     * @warning you must deallocate by yourself the object returned
     * @return an empty clone of the current object with the same type */
    virtual EliminationSequenceStrategy* newFactory() const = 0;

    /// virtual copy constructor
    /** @return a full clone of the current object */
    virtual EliminationSequenceStrategy* copyFactory() const = 0;

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// sets a new graph to be triangulated
    /** The elimination sequence algorithms reinitializes its data to start a
     * new triangulation with graph Graph
     * @param graph the new graph to be triangulated
     * @param dom_sizes the domain sizes of the variables/nodes
     * @return true if the data structures were modified (if the graph or the
     * domain sizes did not change, then there is no need to update the
     * data structures).
     * @warning Note that we allow dom_sizes to be defined over nodes/variables
     * that do not belong to graph. These sizes will simply be ignored. However,
     * it is compulsory that all the nodes of graph belong to dom_sizes
     * @warning the graph can be altered during the triangulation.
     * @warning note that, by aGrUM's rule, the graph and the domain sizes
     * are not copied but only referenced by the elimination sequence algorithm.
     */
    virtual bool setGraph(UndiGraph* graph, const NodeProperty< Size >* dom_sizes);

    /// returns the new node to be eliminated within the triangulation algorithm
    /** @throws NotFound exception is thrown if there is no more node to
     * eliminate in the graph */
    virtual NodeId nextNodeToEliminate() = 0;

    /** @brief if the elimination sequence is able to compute fill-ins, we
     *indicate
     * whether we want this feature to be activated
     *
     * @param do_it when true and the elimination sequence has the ability to
     * compute fill-ins, the elimination sequence will actually compute them
     *(for
     * the triangulation to use them), else they will not be available. */
    virtual void askFillIns(bool do_it) = 0;

    /** @brief indicates whether the fill-ins generated by the eliminated
     * nodes, if needed, will be computed by the elimination sequence, or need
     *be
     * computed by the triangulation itself.
     *
     * An elimination sequence provides fill-ins to its triangulation if and
     * only if it has the ability to compute them and it has been asked to do so
     * (by method askFillIns) */
    virtual bool providesFillIns() const = 0;

    /** @brief indicates whether the elimination sequence updates by itself the
     * graph after a node has been eliminated
     *
     * Some algorithms have more informations than the triangulation algorithm
     * to update the graph after a node has been eliminated. They can thus
     * exploit these informations to update the graph faster than the
     * triangulation itself. Hence the latter should delegate this operation
     * to the elimination sequence. This is the case, for instance, for the
     * defaultEliminationSequenceStrategy, which uses a SimplicialSet that knows
     * that some eliminated nodes do not require any fill-in. */
    virtual bool providesGraphUpdate() const = 0;

    /// performs all the graph/fill-ins updates provided (if any)
    /** @param node the node the elimination of which requires the graph update
     */
    virtual void eliminationUpdate(const NodeId node);

    /** @brief in case fill-ins are provided, this function returns the fill-ins
     * due to all the nodes eliminated so far */
    virtual const EdgeSet& fillIns();

    /// clears the sequence (to prepare, for instance, a new elimination
    /// sequence)
    virtual void clear();

    /// returns the current graph
    UndiGraph* graph() const noexcept;

    /// returns the current domain sizes
    const NodeProperty< Size >* domainSizes() const noexcept;

    /// @}


    protected:
    /// the graph to be triangulated
    UndiGraph* graph_{nullptr};

    /// the domain sizes of the variables/nodes
    const NodeProperty< Size >* domain_sizes_{nullptr};

    /// the log of the domain sizes of the variables/nodes
    NodeProperty< double > log_domain_sizes_;


    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    EliminationSequenceStrategy();

    /// constructor for a (tensorly) non empty graph
    EliminationSequenceStrategy(UndiGraph* graph, const NodeProperty< Size >* domain_sizes);

    /// copy constructor
    EliminationSequenceStrategy(const EliminationSequenceStrategy& from);

    /// move constructor
    EliminationSequenceStrategy(EliminationSequenceStrategy&& from);

    /// @}

    private:
    /// an empty fill-ins set used by default
    static const EdgeSet& _empty_fill_ins_();
  };


} /* namespace gum */


#ifndef GUM_NO_INLINE
#  include <agrum/base/graphs/algorithms/triangulations/eliminationStrategies/eliminationSequenceStrategy_inl.h>
#endif   // GU%_NO_INLINE


#endif   /* GUM_ELIMINATION_SEQUENCE_STRATEGY_H */
