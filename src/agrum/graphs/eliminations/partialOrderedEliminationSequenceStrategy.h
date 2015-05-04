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
 * can be chosen. But all the nodes of the first subset must be eliminated before
 * the nodes of the second, which must be eliminated before those of the third
 * subset, and so on.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_PARTIAL_ORDERED_ELIMINATION_SEQUENCE_STRATEGY_H
#define GUM_PARTIAL_ORDERED_ELIMINATION_SEQUENCE_STRATEGY_H

#include <agrum/core/list.h>
#include <agrum/graphs/eliminations/eliminationSequenceStrategy.h>
#include <agrum/graphs/undiGraph.h>

namespace gum {

  /** @class PartialOrderedEliminationSequenceStrategy
   * @brief Base class for all elimination sequence algorithm that impose a given
   * partial ordering on the nodes elimination sequence, that is, the set of all
   * the nodes is divided into several subsets. Within each subset, any ordering
   * can be chosen. But all the nodes of the first subset must be eliminated before
   * the nodes of the second, which must be eliminated before those of the third
   * subset, and so on.
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

    /** @brief creates a new elimination sequence of the same type as the current
     * object, but this sequence contains only an empty graph
     * @warning you must deallocate by yourself the object returned
     * @return an empty clone of the current object with the same type */
    virtual PartialOrderedEliminationSequenceStrategy *newFactory() const = 0;

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// sets a new graph to be triangulated
    /** The elimination sequence algorithms reinitializes its data to start a new
     * triangulation with graph Graph
     * @param graph the new graph to be triangulated
     * @param dom the modalities of the nodes (i.e., their domain sizes)
     * @param subsets the list of the subsets constituting the partial ordering
     * @warning note that, by aGrUM's rule, the graph and the sequence are not
     * copied but only referenced by the elimination sequence algorithm. */
    virtual void setGraph(UndiGraph *graph, const NodeProperty<Size> *dom,
                          const List<NodeSet> *subsets) = 0;

    /// @}

    protected:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor (uses an empty graph)
    PartialOrderedEliminationSequenceStrategy();

    /// copy constructor
    PartialOrderedEliminationSequenceStrategy(
        const PartialOrderedEliminationSequenceStrategy &);

    /// @}
  };

} /* namespace gum */

#endif /* GUM_PARTIAL_ORDERED_ELIMINATION_SEQUENCE_STRATEGY_H */
