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
 * @brief class for graph triangulations for which we enforce a given complete
 * ordering on the nodes eliminations.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_ORDERED_TRIANGULATION_H
#define GUM_ORDERED_TRIANGULATION_H

#include <vector>

#include <agrum/graphs/triangulations/eliminationStrategies/orderedEliminationSequenceStrategy.h>
#include <agrum/graphs/triangulations/junctionTreeStrategies/defaultJunctionTreeStrategy.h>
#include <agrum/graphs/triangulations/staticTriangulation.h>

namespace gum {


  /** @class OrderedTriangulation
   * @brief class for graph triangulations for which we enforce a given complete
   * ordering on the nodes eliminations.
   *
   * \ingroup graph_group
   *
   */
  class OrderedTriangulation : public StaticTriangulation {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** @param elimSeq the elimination sequence used to triangulate the graph
     * @param JTStrategy the junction tree strategy used to create junction
     * trees
     * @param minimality a Boolean indicating whether we should enforce that
     * the triangulation is minimal w.r.t. inclusion */
    OrderedTriangulation(
        const OrderedEliminationSequenceStrategy& elimSeq =
            OrderedEliminationSequenceStrategy(),
        const JunctionTreeStrategy& JTStrategy = DefaultJunctionTreeStrategy(),
        bool                        minimality = false );

    /// constructor with a given graph
    /** @param graph the graph to be triangulated, i.e., the nodes of which will
     * be eliminated
     * @param domsizes the domain sizes of the nodes to be eliminated
     * @param sequence  the order in which the nodes should be eliminated
     * @param elimSeq the elimination sequence used to triangulate the graph.
     * Only a reference/pointer to the sequence passed in argument is kept in
     * the current class.
     * @param JTStrategy the junction tree strategy used to create junction
     * trees
     * @param minimality a Boolean indicating whether we should enforce that
     * the triangulation is minimal w.r.t. inclusion
     * @warning Note that we allow dom_sizes to be defined over nodes/variables
     * that do not belong to graph. These sizes will simply be ignored. However,
     * it is compulsory that all the nodes of graph belong to dom_sizes
     * @warning note that, by aGrUM's rule, the graph, the domain sizes and the
     * elimination sequence are not copied but only referenced by the
     * triangulation algorithm. */
    OrderedTriangulation(
        const UndiGraph*                          graph,
        const NodeProperty<Size>*                 domsizes,
        const std::vector<NodeId>*                sequence,
        const OrderedEliminationSequenceStrategy& elimSeq =
            OrderedEliminationSequenceStrategy(),
        const JunctionTreeStrategy& JTStrategy = DefaultJunctionTreeStrategy(),
        bool                        minimality = false );

    /// copy constructor
    OrderedTriangulation( const OrderedTriangulation& from );

    /// move constructor
    OrderedTriangulation( OrderedTriangulation&& from );

    /** @brief returns a fresh triangulation over the same graph and of the same
     * type as the current object (using the same sequence, etc.)
     *
     * note that we return a pointer as it enables subclasses to return
     * pointers to their types, not Triangulation pointers. See item 25 of the
     * more effective C++.*/
    virtual OrderedTriangulation* newFactory() const final;

    /// virtual copy constructor
    virtual OrderedTriangulation* copyFactory() const final;

    /// destructor
    virtual ~OrderedTriangulation();

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// initialize the triangulation data structures for a new graph
    /** @param graph the graph to be triangulated, i.e., the nodes of which will
     * be eliminated
     * @param domsizes the domain sizes of the nodes to be eliminated
     * @param sequence  the order in which the nodes should be eliminated
     * @warning note that, by aGrUM's rule, the graph and the domain sizes are
     * not
     *  notcopied but only referenced by the triangulation algorithm. */
    virtual void setGraph( const UndiGraph*          graph,
                           const NodeProperty<Size>* domsizes ) final;

    /// sets the sequence of elimination (only the reference is stored)
    /** The elimination sequence is kept outside this class for efficiency
     * reasons. */
    virtual void setOrder( const std::vector<NodeId>* order ) final;

    /// @}

    protected:
    /// the function called to initialize the triangulation process
    /** This function is called when the triangulation process starts and is
     * used to initialize the elimination sequence strategy. Actually, the
     * graph that is modified by the triangulation algorithm is a copy of
     * the original graph, and this copy need be known by the elimination
     * sequence strategy. _initTriangulation is used to transmit this
     * knowledge to the elimination sequence (through method setGraph of the
     * elimination sequence class).
     * @param graph the very graph that is triangulated (this is a copy of
     * _original_graph) */
    virtual void _initTriangulation( UndiGraph& graph ) final;


    /// the elimination sequence to apply
    /** @warning __order is not owned by the orderedTriangulation class */
    const std::vector<NodeId>* __order{nullptr};

    /// @}

    private:
    /// forbid copy operator
    OrderedTriangulation& operator=( const OrderedTriangulation& );
  };

} /* namespace gum */

#endif /* GUM_ORDERED_TRIANGULATION_H */
