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
 * @brief Class for computing default triangulations of graphs
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_DEFAULT_TRIANGULATION_H
#define GUM_DEFAULT_TRIANGULATION_H

#include <agrum/base/graphs/algorithms/simplicialSet.h>
#include <agrum/base/graphs/algorithms/triangulations/unconstrainedTriangulation.h>

namespace gum {

  /** @class DefaultTriangulation
   * @brief The default triangulation algorithm used by aGrUM
   *
   * \ingroup graph_group
   *
   * By default, this is the very class used by aGrUM for performing
   * triangulations. The algorithm used is the following:
   * # the graph passed in argument is completed by fill-ins until it becomes
   *   triangulated
   * # then an elimination tree is computed from this triangulated graph
   * # finally, a junction tree is derived from the elimination tree
   *
   * The triangulation step first tries to remove simplicial nodes, that is,
   * nodes that belong to only one clique. Then almost simplicial nodes of low
   * width are removed (almost simplicial nodes are nodes such that all but
   * one of their neighbors form a clique). Then quasi simplicial nodes are
   * removed, that is, nodes such that the ratio of the number of fill-ins to
   * add to form a clique by the number of edges in a clique is small. Then
   * nodes that create cliques of small weight are removed.
   *
   * The transformation from the elimination tree to the join tree is performed
   * bottom-up. Each time a node of the elimination tree is identified to be a
   * sub-clique, it is removed and all of its parents but one are linked to the
   * latter. The identification of sub-cliques is very fast (comparison
   * of 2 ints).
   */
  class DefaultTriangulation: public UnconstrainedTriangulation {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// basic constructor. initialize the triangulation
    explicit DefaultTriangulation(const UndiGraph*            graph,
                                  const NodeProperty< Size >* dom_sizes,
                                  bool                        minimality   = false,
                                  double                      theRatio     = GUM_QUASI_RATIO,
                                  double                      theThreshold = GUM_WEIGHT_THRESHOLD);

    /// default constructor: initialize the triangulation for an empty graph
    explicit DefaultTriangulation(bool   minimality   = false,
                                  double theRatio     = GUM_QUASI_RATIO,
                                  double theThreshold = GUM_WEIGHT_THRESHOLD);

    /// copy constructor
    DefaultTriangulation(const DefaultTriangulation& from);

    /// move constructor
    DefaultTriangulation(DefaultTriangulation&& from);

    /// destructor
    ~DefaultTriangulation();

    /// virtual clone constructor
    /** returns a fresh triangulation (over an empty graph) of the same
     * type as the current object
     *
     * note that we return a pointer as it enables subclasses to return
     * pointers to their types, not Triangulation pointers. See item 25 of the
     * more effective C++.*/
    virtual DefaultTriangulation* newFactory() const;

    /// virtual copy constructor
    virtual DefaultTriangulation* copyFactory() const;

    /// @}


    private:
    /// the ratio above which we consider nodes to be quasi simplicial
    double _quasi_ratio_;

    /** @brief threshold under which almost and quasi simplicial nodes can be
     * chosen to be eliminated */
    double _threshold_;


    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// forbid copy operator
    DefaultTriangulation& operator=(const DefaultTriangulation&);

    /// @}
  };


} /* namespace gum */


#endif /* GUM_DEFAULT_TRIANGULATION_H */
