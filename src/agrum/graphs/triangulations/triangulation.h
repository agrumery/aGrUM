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
 * @brief Abstract base class for computing triangulations of graphs
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_TRIANGULATION_H
#define GUM_TRIANGULATION_H

#include <vector>
#include <agrum/graphs/cliqueGraph.h>

namespace gum {

  /* =========================================================================== */
  /* ===               BASIC INTERFACE TO PERFORM TRIANGULATIONS             === */
  /* =========================================================================== */
  /** @class Triangulation
   * @brief Interface for all triangulation methods
   *
   * \ingroup graph_group
   *
   */
  /* =========================================================================== */
  class Triangulation {
    public:
    /// constructor

    Triangulation();

    /// destructor

    virtual ~Triangulation();

    /// returns the fill-ins added by the triangulation algorithm

    virtual const EdgeSet &fillIns() = 0;

    /// returns an elimination ordering compatible with the triangulated graph
    /// @todo : whu not a Sequence ?

    virtual const std::vector<NodeId> &eliminationOrder() = 0;

    /** @brief returns the number of a given node in the elimination order
     * (0 = first node eliminated) */

    virtual Idx eliminationOrder(const NodeId) = 0;

    /// returns the triangulated graph

    virtual const UndiGraph &triangulatedGraph() = 0;

    /// returns the elimination tree of a compatible ordering

    virtual const CliqueGraph &eliminationTree() = 0;

    /// returns a compatible junction tree

    virtual const CliqueGraph &junctionTree() = 0;

    /// returns the max of log10DomainSize of cliques in the junction tree.
    /** This is usefull for instance to estimate the complexity (both in space and in
     *time) of
     * the inference that will use the junction tree.
     *
     * This method is not 'const' since it can be called before building any junction
     *tree
     * and hence needs to build it...
     **/

    double maxLog10CliqueDomainSize();

    /** @brief returns the Id of the clique created by the
     * elimination of a given node during the triangulation process */

    virtual NodeId createdJunctionTreeClique(const NodeId id) = 0;

    /** @brief returns the Ids of the cliques of the junction tree created by the
     * elimination of the nodes */

    virtual const NodeProperty<NodeId> &createdJunctionTreeCliques() = 0;

    /// returns a junction tree of maximal prime subgraphs
    /** @warning Actually, the cliques of the junction tree are guarranteed to be
     * maximal prime subgraph of the original graph that was triangulated only if
     * the triangulation performed is minimal (in the sense that removing any edge
     * in the triangulated graph results in a nontriangulated graph). This can be
     * ensured by requiring minimality of the triangulation. */

    virtual const CliqueGraph &maxPrimeSubgraphTree() = 0;

    /** @brief returns the Id of the maximal prime subgraph created by the
     * elimination of a given node during the triangulation process */

    virtual NodeId createdMaxPrimeSubgraph(const NodeId id) = 0;

    /// reinitialize the graph to be triangulated to an empty graph

    virtual void clear() = 0;

    /** @brief returns a fresh triangulation (over an empty graph) of the same
     * type as the current object
     *
     * note that we return a pointer as it enables subclasses to return
     * pointers to their types, not Triangulation pointers. See item 25 of the
     * more effective C++.*/

    virtual Triangulation *newFactory() const = 0;

    /// returns the modalities of the variables of the graph to be triangulated

    const NodeProperty<Size> &modalities() const;

    protected:
    /// the modalities of the nodes of the graph
    NodeProperty<Size> _modalities;

    private:
    /// prevent copy constructor
    Triangulation(const Triangulation &);

    /// prevent copy operator
    Triangulation &operator=(const Triangulation &);
  };

} /* namespace gum */

#endif /* GUM_TRIANGULATION_H */
