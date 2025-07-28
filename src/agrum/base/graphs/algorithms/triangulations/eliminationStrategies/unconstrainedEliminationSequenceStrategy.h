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
 * @brief Base Class for all elimination sequence algorithms that require only
 * the graph to be triangulated and the nodes' domain sizes to produce the node
 * elimination ordering.
 *
 * In many cases, the graph to be triangulated and the nodes domain sizes are
 * the only information needed to compute an elimination ordering. For instance,
 * this is only what is needed for computing junction trees for Bayesian
 * networks or GAI networks. For such cases, the elimination sequence algorithms
 * derived from UnconstrainedEliminationSequenceStrategy are appropriate. There
 * are however other cases in which additional informations must be passed to
 * the elimination sequence algorithm to produce a correct elimination ordering.
 * For instance, computing a strong junction tree for an influence diagram
 * requires the knowledge of a partial ordering on the nodes to be eliminated.
 * For such cases, classes derived from UnconstrainedEliminationSequenceStrategy
 * are not appropriate and those should be derived from other elimination
 * sequence
 * factories, for from instance PartialOrderedEliminationSequenceStrategy.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_UNCONSTRAINED_ELIMINATION_SEQUENCE_STRATEGY_H
#define GUM_UNCONSTRAINED_ELIMINATION_SEQUENCE_STRATEGY_H

#include <agrum/base/graphs/algorithms/triangulations/eliminationStrategies/eliminationSequenceStrategy.h>

namespace gum {

  /** @class UnconstrainedEliminationSequenceStrategy
   * @brief The base class for all elimination sequence algorithms that require
   * only the graph to be triangulated and the nodes' domain sizes to produce
   * the node elimination ordering.
   *
   * \ingroup graph_group
   *
   */
  class UnconstrainedEliminationSequenceStrategy: public EliminationSequenceStrategy {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// destructor
    virtual ~UnconstrainedEliminationSequenceStrategy();

    /** @brief creates a new elimination sequence of the same type as the
     * current object, but this sequence contains only an empty graph
     * @warning you must deallocate by yourself the object returned
     * @return an empty clone of the current object with the same type */
    virtual UnconstrainedEliminationSequenceStrategy* newFactory() const = 0;

    /// virtual copy constructor
    virtual UnconstrainedEliminationSequenceStrategy* copyFactory() const = 0;

    /// @}


    protected:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    UnconstrainedEliminationSequenceStrategy();

    /// constructor for a (tensorly) non empty graph
    /** @param graph the graph to be triangulated, i.e., the nodes of which will
     * be eliminated
     * @param dom_sizes the domain sizes of the nodes to be eliminated
     * @warning Note that we allow dom_sizes to be defined over nodes/variables
     * that do not belong to graph. These sizes will simply be ignored. However,
     * it is compulsory that all the nodes of graph belong to dom_sizes
     * @warning the graph is altered during the triangulation.
     * @warning note that, by aGrUM's rule, the graph and the domain sizes are
     * not copied but only referenced by the elimination sequence algorithm. */
    UnconstrainedEliminationSequenceStrategy(UndiGraph*                  graph,
                                             const NodeProperty< Size >* dom_sizes);

    /// copy constructor
    UnconstrainedEliminationSequenceStrategy(const UnconstrainedEliminationSequenceStrategy&);

    /// move constructor
    UnconstrainedEliminationSequenceStrategy(UnconstrainedEliminationSequenceStrategy&&);

    /// @}
  };

} /* namespace gum */

#endif /* GUM_UNCONSTRAINED_ELIMINATION_SEQUENCE_STRATEGY_H */
