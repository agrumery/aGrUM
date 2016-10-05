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

#include <agrum/graphs/triangulations/junctionTreeStrategies/junctionTreeStrategy.h>

namespace gum {

  /** @class DefaultJunctionTreeStrategy
   * @brief An algorithm producing a junction given the elimination tree
   * produced by a triangulation algorithm
   *
   * \ingroup graph_group
   *
   */
  class DefaultJunctionTreeStrategy : public JunctionTreeStrategy {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    DefaultJunctionTreeStrategy();

    /// copy constructor
    DefaultJunctionTreeStrategy( const DefaultJunctionTreeStrategy& from );

    /// move constructor
    DefaultJunctionTreeStrategy( DefaultJunctionTreeStrategy&& from );

    /// destructor
    virtual ~DefaultJunctionTreeStrategy();

    /// create a clone not assigned to any triangulation algorithm
    virtual DefaultJunctionTreeStrategy* newFactory() const final;

    /// virtual copy constructor
    /** @param triangulation if triangulation is different from nullptr, this
     * becomes the new triangulation algorithm associated with the junction tree
     * strategy */
    virtual DefaultJunctionTreeStrategy*
    copyFactory( StaticTriangulation* triangulation = nullptr ) const final;

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /** @brief indicates whether the junction tree strategy needs fill-ins
     * to work properly
     *
     * If the junctionTreeStrategy needs fill-ins to work properly, its
     * assigned triangulation instance (see method setTriangulation) will be
     * commited to compute them. */
    virtual bool requiresFillIns() const final;

    /// returns the junction tree computed
    /** The idea behind this method is that the JunctionTreeStrategy asks its
     * assigned triangulation (see method setTriangulation) all the data it
     * needs to compute correctly the junction tree. For instance, it may asks
     * for a triangulated graph or an elimination tree, or even the order of
     * elimination of the nodes, etc. All these data are available from the
     * triangulation class. Knowing these data, the junctionTreeStrategy
     * computes and returns a junction tree corresponding to the triangulated
     * graph.
     * @throws UndefinedElement is raised if no triangulation has been assigned
     * to the DefaultJunctionTreeStrategy */
    virtual const CliqueGraph& junctionTree() final;

    /// assigns the triangulation to the junction tree strategy
    /** @param the triangulation whose resulting cliques will be used to
     * construct the junction tree
     * @warning note that, by aGrUM's rule, the graph and the domain sizes
     * are not copied but only referenced by the junction tree strategy. */
    virtual void setTriangulation( StaticTriangulation* triangulation ) final;

    /** @brief returns, for each node, the clique of the junction tree which was
     * created by its deletion
     * @throws UndefinedElement is raised if no triangulation has been assigned
     * to the DefaultJunctionTreeStrategy */
    virtual const NodeProperty<NodeId>& createdCliques() final;

    /** @brief returns the Id of the clique of the junction tree created by the
     * elimination of a given node during the triangulation process
     * @param id the id of the node in the original undirected graph whose
     * created clique's id is looked for
     * @throws UndefinedElement is raised if no triangulation has been assigned
     * to the DefaultJunctionTreeStrategy */
    virtual NodeId createdClique( const NodeId id ) final;

    /// resets the current junction tree strategy data structures
    virtual void clear() final;

    /// @}

    private:
    /// a boolean indicating whether the junction tree has been constructed
    bool __has_junction_tree{false};

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
