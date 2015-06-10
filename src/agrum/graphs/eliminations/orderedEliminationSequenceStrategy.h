/***************************************************************************
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
/** @file
 * @brief An Elimination sequence algorithm that imposes a given complete ordering
 * on the nodes elimination sequence
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_ORDERED_ELIMINATION_SEQUENCE_STRATEGY_H
#define GUM_ORDERED_ELIMINATION_SEQUENCE_STRATEGY_H

#include <vector>
#include <agrum/graphs/eliminations/eliminationSequenceStrategy.h>
#include <agrum/graphs/undiGraph.h>

namespace gum {

  /** @class OrderedEliminationSequenceStrategy
   * @brief An Elimination sequence algorithm that imposes a given complete
   * ordering on the nodes elimination sequence.
   *
   * \ingroup graph_group
   *
   */
  class OrderedEliminationSequenceStrategy : public EliminationSequenceStrategy {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor (uses an empty graph)
    OrderedEliminationSequenceStrategy();

    /// constructor for an a priori non empty graph
    /** @param graph the graph to be triangulated, i.e., the nodes of which will
     * be eliminated
     * @param sequence the order in which the nodes should be eliminated
     * @warning note that, by aGrUM's rule, the graph and the sequence are not
     * copied but only referenced by the elimination sequence algorithm. */
    OrderedEliminationSequenceStrategy(UndiGraph *graph,
                                       const std::vector<NodeId> *sequence);

    /// copy constructor
    OrderedEliminationSequenceStrategy(const OrderedEliminationSequenceStrategy &);

    /// destructor
    virtual ~OrderedEliminationSequenceStrategy();

    /** @brief creates a new elimination sequence of the same type as the current
     * object, but this sequence contains only an empty graph
     * @warning you must deallocate by yourself the object returned
     * @return an empty clone of the current object with the same type */
    virtual OrderedEliminationSequenceStrategy *newFactory() const;

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// sets a new graph to be triangulated
    /** The elimination sequence algorithms reinitializes its data to start a new
     * triangulation with graph Graph
     * @param graph the new graph to be triangulated
     * @param sequence the order in which the nodes should be eliminated
     * @warning note that, by aGrUM's rule, the graph and the sequence are not
     * copied but only referenced by the elimination sequence algorithm. */
    void setGraph(UndiGraph *graph, const std::vector<NodeId> *sequence);

    /// clears the sequence (to prepare, for instance, a new elimination sequence)
    void clear();

    /// returns the new node to be eliminated within the triangulation algorithm
    /** @throws NotFound exception is thrown if there is no more node to
     * eliminate in the graph */
    NodeId nextNodeToEliminate();

    /** @brief if the elimination sequence is able to compute fill-ins, we indicate
     * whether we want this feature to be activated
     *
     * @param do_it when true and the elimination sequence has the ability to
     * compute fill-ins, the elimination sequence will actually compute them (for
     * the triangulation to use them), else they will not be available. */
    void askFillIns(bool do_it);

    /** @brief indicates whether the fill-ins generated by the eliminated
     * nodes, if needed, will be computed by the elimination sequence, or need be
     * computed by the triangulation itself.
     *
     * An elimination sequence provides fill-ins to its triangulation if and
     * only if it has the ability to compute them and it has been asked to do so
     * (by method askFillIns) */
    bool providesFillIns() const;

    /** @brief indicates whether the elimination sequence updates by itself the
     * graph after a node has been eliminated */
    bool providesGraphUpdate() const;

    /// performs all the graph/fill-ins updates provided (if any)
    /** @param node the node the elimination of which requires the graph update */
    void eliminationUpdate(const NodeId node);

    /** @brief in case fill-ins are provided, this function returns the fill-ins
     * due to all the nodes eliminated so far */
    const EdgeSet &fillIns();

    /// @}

    private:
    /// the graph to triangulate
    const UndiGraph *__graph;

    /// the vector indicating in which order we should eliminate the nodes
    const std::vector<NodeId> *__sequence;

    /// the index in the sequence indicating the new node to eliminate
    unsigned int __seq_index;
  };

} /* namespace gum */

#endif /* GUM_ORDERED_ELIMINATION_SEQUENCE_STRATEGY_H */
