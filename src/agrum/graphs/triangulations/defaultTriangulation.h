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
 * @brief Class for computing default triangulations of graphs
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_DEFAULT_TRIANGULATION_H
#define GUM_DEFAULT_TRIANGULATION_H

#include <agrum/graphs/simplicialSet.h>
#include <agrum/graphs/triangulations/unconstrainedTriangulation.h>

namespace gum {

  /* =========================================================================== */
  /* ===             BASIC TRIANGULATION ALGORITHM USED BY AGRUM             === */
  /* =========================================================================== */
  /** @class DefaultTriangulation
   * @brief The default triangulation algorithm used by aGrUM
   *
   * \ingroup graph_group
   *
   *
   * By default, this is the very class used by aGrUM for performing
   * triangulations. The algorithm used is the following:
   * # the graph passed in argument is completed by fill-ins until it becomes
   *   triangulated
   * # then an elimination tree is computed from this triangulated graph
   * # finally, a junction tree is derived from the elimination tree
   *
   * The triangulation step first tries to remove simplicial nodes, that is, nodes
   * that belong to only one clique. Then almost simplicial nodes of low width are
   * removed (almost simplicial nodes are nodes such that all but one of their
   * neighbours form a clique). Then quasi simplicial nodes are removed, that is,
   * nodes such that the ratio of the number of fill-ins to add to form a clique
   * by the number of edges in a clique is small. Then nodes that create cliques
   * of small weight are removed.
   *
   * The transformation from the elimination tree to the join tree is performed
   * bottom-up. Each time a node of the elimination tree is identified to be a
   * sub-clique, it is removed and all of its parents but one are linked to the
   * latter. The identification of sub-cliques is very fast (comparison of 2 ints).
   */
  /* =========================================================================== */
  class DefaultTriangulation : public UnconstrainedTriangulation {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// basic constructor. initialize the triangulation

    explicit DefaultTriangulation(const UndiGraph *graph,
                                  const NodeProperty<Size> *modal,
                                  bool minimality = false,
                                  float theRatio = GUM_QUASI_RATIO,
                                  float theThreshold = GUM_WEIGHT_THRESHOLD);

    /// default constructor: initialize the triangulation for an empty graph

    explicit DefaultTriangulation(bool minimality = false,
                                  double theRatio = GUM_QUASI_RATIO,
                                  double theThreshold = GUM_WEIGHT_THRESHOLD);

    /// copy constructor

    // DefaultTriangulation( const DefaultTriangulation& from );

    /// destructor

    ~DefaultTriangulation();

    /// virtual copy constructor
    /** returns a fresh triangulation (over an empty graph) of the same
     * type as the current object
     *
     * note that we return a pointer as it enables subclasses to return
     * pointers to their types, not Triangulation pointers. See item 25 of the
     * more effective C++.*/

    virtual DefaultTriangulation *newFactory() const;

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator

    DefaultTriangulation &operator=(const DefaultTriangulation &);

    /// @}

    private:
    /// the ratio above which we consider nodes to be quasi simplicial
    float __quasi_ratio;

    /** @brief threshold under which almost and quasi simplicial nodes can be
     * chosen to be eliminated */
    float __threshold;
  };

} /* namespace gum */

#endif /* GUM_DEFAULT_TRIANGULATION_H */
