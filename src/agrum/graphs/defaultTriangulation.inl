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
 * @brief Inline implementations for computing default triangulations of graphs
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {


  /* =========================================================================== */
  /* =========================================================================== */
  /* ===            DEFAULT TRIANGULATION ALGORITHM USED BY AGRUM            === */
  /* =========================================================================== */
  /* =========================================================================== */

  // ==============================================================================
  /// returns the fill-ins added by the triangulation algorithm
  // ==============================================================================
  INLINE
  const EdgeSet& DefaultTriangulation::fillIns() {
    if ( !__has_triangulation )  __triangulate();

    if ( __minimality_required || ( ! __we_want_fill_ins ) ) {
      return __fill_ins;
    }
    else {
      return __fill_ins_list_standard;
    }
  }

  // ==============================================================================
  /// returns an elimination ordering compatible with the triangulated graph
  // ==============================================================================
  INLINE
  const std::vector<NodeId>& DefaultTriangulation::eliminationOrder() {
    if ( !__has_triangulation )  __triangulate();

    return __elim_order;
  }

  // ==============================================================================
  /** @brief returns the number of a given node in the elimination order
   * (0 = first node eliminated) */
  // ==============================================================================
  INLINE
  unsigned int DefaultTriangulation::eliminationOrder( const NodeId id ) {
    if ( !__has_triangulation )  __triangulate();

    return __reverse_elim_order[id];
  }

  // ==============================================================================
  /// returns the elimination tree of a compatible ordering
  // ==============================================================================
  INLINE
  const CliqueGraph&
  DefaultTriangulation::eliminationTree() {
    if ( !__has_elimination_tree )  __computeEliminationTree();

    return __elim_tree;
  }

  // ==============================================================================
  /// returns a compatible junction tree
  // ==============================================================================
  INLINE
  const CliqueGraph&
  DefaultTriangulation::junctionTree() {
    __computeJunctionTree(); // checks if junctionTree already exists
    return __junction_tree;
  }

  // ==============================================================================
  /// returns a junction tree of maximal prime subgraphs
  // ==============================================================================
  INLINE
  const CliqueGraph&
  DefaultTriangulation::maxPrimeSubgraphTree() {
    if ( !__has_max_prime_junction_tree )  __computeMaxPrimeJunctionTree();

    return __max_prime_junction_tree;
  }

  // ==============================================================================
  /** @brief returns the Id of the maximal prime subgraph created by the
   * elimination of a given node during the triangulation process */
  // ============================================================================
  INLINE
  NodeId DefaultTriangulation::createdMaxPrimeSubgraph( const NodeId id ) {
    if ( !__has_max_prime_junction_tree )  __computeMaxPrimeJunctionTree();

    return __node_2_max_prime_clique[id];
  }

  // ==============================================================================
  /** @brief returns the Id of the clique created by the
   * elimination of a given node during the triangulation process */
  // ==============================================================================
  INLINE
  NodeId DefaultTriangulation::createdClique( const NodeId id ) {
    if ( !__has_junction_tree )  __computeJunctionTree();

    return __node_2_junction_clique[id];
  }

  // ==============================================================================
  /// sets/unset the fill-ins storage in the standard triangulation procedure
  // ==============================================================================
  INLINE
  void DefaultTriangulation::setFillIns( bool b ) {
    __we_want_fill_ins = b;
  }


} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
