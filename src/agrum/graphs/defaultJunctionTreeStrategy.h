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
 * @brief An algorithms producing a junction given the elimination tree
 * produced by the triangulation algorithm
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_DEFAULT_JUNCTION_TREE_STRATEGY_H
#define GUM_DEFAULT_JUNCTION_TREE_STRATEGY_H

#include <agrum/graphs/junctionTreeStrategy.h>

namespace gum {

  /* =========================================================================== */
  /** @class DefaultJunctionTreeStrategy
   * @brief An algorithm producing a junction given the elimination tree
   * produced by a triangulation algorithm
   *
   * \ingroup graph_group
   *
   */
  /* =========================================================================== */
  class DefaultJunctionTreeStrategy : public JunctionTreeStrategy {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    DefaultJunctionTreeStrategy();

    /// destructor
    virtual ~DefaultJunctionTreeStrategy();

    /// virtual copy constructor
    virtual DefaultJunctionTreeStrategy *newFactory() const;

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// indicates whether the junction tree strategy needs fill-ins to work properly
    /** If the junctionTreeStrategy needs fill-ins to work properly, its assigned
     * triangulation instance (see method setTriangulation) will be commited to
     * compute them. */
    bool requiresFillIns() const;

    /// returns the junction tree computed
    /** The idea behind this method is that the JunctionTreeStrategy asks its
     * assigned triangulation (see method setTriangulation) all the data it
     * needs to compute correctly the junction tree. For instance, it may asks
     * for a triangulated graph or an elimination tree, or even the order of
     * elimination of the nodes, etc. All these data are available from the
     * triangulation class. Knowing these data, the junctionTreeStrategy computes
     * and returns a junction tree corresponding to the triangulated graph. */
    const CliqueGraph &junctionTree();

    /// assign the triangulation to the junction tree strategy
    void setTriangulation(StaticTriangulation *triangulation);

    /** @brief returns, for each node, the clique of the junction tree which was
     * created by its deletion */
    const NodeProperty<NodeId> &createdCliques();

    /** @brief returns the Id of the clique of the junction tree created by the
     * elimination of a given node during the triangulation process */
    NodeId createdClique(const NodeId id);

    /// resets the current junction tree strategy data structures
    void clear();

    /// @}

    private:
    /// the triangulation to which the junction tree is associated
    StaticTriangulation *__triangulation;

    /// a boolean indicating whether the junction tree has been constructed
    bool __has_junction_tree;

    /// the junction tree computed by the algorithm
    CliqueGraph __junction_tree;

    /** @brief indicates which clique of the junction tree was created
     * by the elimination of a given node (the key of the table) */
    NodeProperty<NodeId> __node_2_junction_clique;

    /// computes a junction tree from an elimination tree
    void __computeJunctionTree();
  };

} /* namespace gum */

#endif /* GUM_DEFAULT_JUNCTION_TREE_STRATEGY_H */
