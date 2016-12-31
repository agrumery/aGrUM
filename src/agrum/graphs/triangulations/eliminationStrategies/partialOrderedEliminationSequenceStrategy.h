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
 * @brief Base class for all elimination sequence algorithm that impose a given
 * partial ordering on the nodes elimination sequence, that is, the set of all
 * the nodes is divided into several subsets. Within each subset, any ordering
 * can be chosen. But all the nodes of the first subset must be eliminated
 * before the nodes of the second, which must be eliminated before those of the
 * third subset, and so on.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_PARTIAL_ORDERED_ELIMINATION_SEQUENCE_STRATEGY_H
#define GUM_PARTIAL_ORDERED_ELIMINATION_SEQUENCE_STRATEGY_H

#include <agrum/core/list.h>
#include <agrum/graphs/triangulations/eliminationStrategies/eliminationSequenceStrategy.h>

namespace gum {

  /** @class PartialOrderedEliminationSequenceStrategy
   * @brief Base class for all elimination sequence algorithm that impose a
   * given
   * partial ordering on the nodes elimination sequence, that is, the set of all
   * the nodes is divided into several subsets. Within each subset, any ordering
   * can be chosen. But all the nodes of the first subset must be eliminated
   * before the nodes of the second, which must be eliminated before those of
   * the
   * third subset, and so on.
   *
   * \ingroup graph_group
   *
   */
  class PartialOrderedEliminationSequenceStrategy
      : public EliminationSequenceStrategy {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// destructor
    virtual ~PartialOrderedEliminationSequenceStrategy();

    /** @brief creates a new elimination sequence of the same type as the
     * current object, but this sequence contains only an empty graph
     * @warning you must deallocate by yourself the object returned
     * @return an empty clone of the current object with the same type */
    virtual PartialOrderedEliminationSequenceStrategy* newFactory() const = 0;

    /// virtual copy constructor
    virtual PartialOrderedEliminationSequenceStrategy* copyFactory() const = 0;

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// sets a new graph to be triangulated
    /** The elimination sequence algorithms reinitializes its data to start a
     * new triangulation with graph Graph
     * @param graph the new graph to be triangulated
     * @param dom_sizes the domain sizes of the nodes/variables
     * @return true if the data structures were modified (if the graph or the
     * domain sizes did not change, then there is no need to update the
     * data structures).
     * @warning Note that we allow dom_sizes to be defined over nodes/variables
     * that do not belong to graph. These sizes will simply be ignored. However,
     * it is compulsory that all the nodes of graph belong to dom_sizes
     * @warning the graph is altered during the triangulation.
     * @warning note that, by aGrUM's rule, the graph and the sequence are not
     * copied but only referenced by the elimination sequence algorithm. */
    virtual bool setGraph( UndiGraph* graph, const NodeProperty<Size>* dom_sizes );

    /// sets a new partial ordering constraint on the elimination sequence
    /** @param subsets the list of the subsets constituting the partial ordering
     * @return true if the new partial order has been successfully assigned
     * (i.e.,
     * if all the nodes of the graph belong to one of the subsets)
     * @warning if the subsets contain some nodes that do not belong to the
     * graph,
     * then these nodes are simply ignored.
     * @warning note that, by aGrUM's rule, the partial ordering is not copied
     * but only referenced by the elimination sequence algorithm. */
    virtual bool setPartialOrder( const List<NodeSet>* subsets );

    /// clears the sequence (to prepare, for instance, a new elimination
    /// sequence)
    virtual void clear();

    /// returns the current partial ordering
    const List<NodeSet>* partialOrder() const noexcept;

    /// indicates if a new partial ordering is needed
    /** if the current partial ordering does not contain all the nodes of the
     * graph or if the graph itself is not defined (nullptr) a new partial
     * ordering will be needed for the next triangulation */
    bool isPartialOrderNeeded() const noexcept;

    /// @}


    protected:
    /// the subsets constituting the partial ordering
    const List<NodeSet>* _subsets{nullptr};

    /// the iterator indicating which is the current subset on which we work
    List<NodeSet>::const_iterator _subset_iter;

    /// the nodes which can be currently eliminated
    NodeSet _nodeset;

    /// indicate whether a new partial ordering is necessary for the elimination
    bool _partial_order_needed{true};


    /// indicate whether a partial ordering is compatible with the current graph
    /** The method checks whether all the nodes of the graph belong to the
     * partial ordering.
     * @return true if some nodes in _graph do not belong to subsets or if
     * _graph is not defnined (nullptr) */
    bool _isPartialOrderNeeded( const List<NodeSet>* subsets ) const;


    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor (uses an empty graph)
    PartialOrderedEliminationSequenceStrategy();

    /// constructor for an a priori non empty graph
    /** @param graph the graph to be triangulated, i.e., the nodes of which will
     * be eliminated
     * @param dom_sizes thedomain sizes of the nodes/variables
     * @param subsets the list of the subsets constituting the partial ordering
     * @warning Note that we allow dom_sizes to be defined over nodes/variables
     * that do not belong to graph. These sizes will simply be ignored. However,
     * it is compulsory that all the nodes of graph belong to dom_sizes
     * @warning the graph is altered during the triangulation.
     * @warning note that, by aGrUM's rule, the graph, the domain sizes and
     * the sequence are not copied but only referenced by the elimination
     * sequence algorithm. */
    PartialOrderedEliminationSequenceStrategy( UndiGraph*                graph,
                                               const NodeProperty<Size>* dom_sizes,
                                               const List<NodeSet>*      subsets );

    /// copy constructor
    PartialOrderedEliminationSequenceStrategy(
        const PartialOrderedEliminationSequenceStrategy& );

    /// move constructor
    PartialOrderedEliminationSequenceStrategy(
        PartialOrderedEliminationSequenceStrategy&& );

    /// @}
  };

} /* namespace gum */


#ifndef GUM_NO_INLINE
#include <agrum/graphs/triangulations/eliminationStrategies/partialOrderedEliminationSequenceStrategy_inl.h>
#endif  // GUM_NOINLINE


#endif /* GUM_PARTIAL_ORDERED_ELIMINATION_SEQUENCE_STRATEGY_H */
