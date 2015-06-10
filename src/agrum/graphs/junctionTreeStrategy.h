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
 * @brief Base Class for all the algorithms producing a junction given a set
 * of cliques/subcliques resulting from a triangulation
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_JUNCTION_TREE_STRATEGY_H
#define GUM_JUNCTION_TREE_STRATEGY_H

#include <agrum/config.h>

#include <agrum/graphs/graphElements.h>
#include <agrum/graphs/cliqueGraph.h>

namespace gum {

  class StaticTriangulation;

  /* =========================================================================== */
  /** @class JunctionTreeStrategy
   * @brief Base Class for all the algorithms producing a junction given a set
   * of cliques/subcliques resulting from a triangulation.
   *
   * \ingroup graph_group
   *
   */
  /* =========================================================================== */
  class JunctionTreeStrategy {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// destructor
    virtual ~JunctionTreeStrategy();

    /// virtual copy constructor
    virtual JunctionTreeStrategy *newFactory() const = 0;

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// indicates whether the junction tree strategy needs fill-ins to work properly
    /** If the junctionTreeStrategy needs fill-ins to work properly, its assigned
     * triangulation instance (see method setTriangulation) will be commited to
     * compute them. */
    virtual bool requiresFillIns() const = 0;

    /// returns the junction tree computed
    /** The idea behind this method is that the JunctionTreeStrategy asks its
     * assigned triangulation (see method setTriangulation) all the data it
     * needs to compute correctly the junction tree. For instance, it may asks
     * for a triangulated graph or an elimination tree, or even the order of
     * elimination of the nodes, etc. All these data are available from the
     * triangulation class. Knowing these data, the junctionTreeStrategy computes
     * and returns a junction tree corresponding to the triangulated graph. */
    virtual const CliqueGraph &junctionTree() = 0;

    /// assign the triangulation to the junction tree strategy
    virtual void setTriangulation(StaticTriangulation *triangulation) = 0;

    /** @brief returns, for each node, the clique of the junction tree which was
     * created by its deletion */
    virtual const NodeProperty<NodeId> &createdCliques() = 0;

    /** @brief returns the Id of the clique of the junction tree created by the
     * elimination of a given node during the triangulation process */
    virtual NodeId createdClique(const NodeId id) = 0;

    /// resets the current junction tree strategy data structures
    virtual void clear() = 0;

    /// @}

    protected:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    JunctionTreeStrategy();

    /// @}
  };

} /* namespace gum */

#endif /* GUM_JUNCTION_TREE_STRATEGY_H */
