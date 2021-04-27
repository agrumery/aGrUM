/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/** @file
 * @brief Inline implementations for computing default triangulations of graphs
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {


  /// returns an elimination ordering compatible with the triangulated graph
  INLINE
  const std::vector< NodeId >& StaticTriangulation::eliminationOrder() {
    if (!has_triangulation__) triangulate__();

    return elim_order__;
  }


  /** @brief returns the index of a given node in the elimination order
   * (0 = first node eliminated) */
  INLINE
  Idx StaticTriangulation::eliminationOrder(const NodeId id) {
    if (!has_triangulation__) triangulate__();

    return reverse_elim_order__[id];
  }


  /** @brief returns the number of a given node in the elimination order
   * (0 = first node eliminated) */
  INLINE
  const NodeProperty< NodeId >& StaticTriangulation::reverseEliminationOrder() {
    if (!has_triangulation__) triangulate__();

    return reverse_elim_order__;
  }


  /// returns the elimination tree of a compatible ordering
  INLINE
  const CliqueGraph& StaticTriangulation::eliminationTree() {
    if (!has_elimination_tree__) computeEliminationTree__();

    return elim_tree__;
  }


  /// returns a compatible junction tree
  INLINE
  const CliqueGraph& StaticTriangulation::junctionTree() {
    // checks if junctionTree already exists
    if (!has_junction_tree__) {
      junction_tree__     = &(junction_tree_strategy_->junctionTree());
      has_junction_tree__ = true;
    }

    return *junction_tree__;
  }


  /// returns a junction tree of maximal prime subgraphs
  INLINE
  const CliqueGraph& StaticTriangulation::maxPrimeSubgraphTree() {
    if (!has_max_prime_junction_tree__) computeMaxPrimeJunctionTree__();

    return max_prime_junction_tree__;
  }


  /** @brief returns the Id of the maximal prime subgraph created by the
   * elimination of a given node during the triangulation process */
  INLINE
  NodeId StaticTriangulation::createdMaxPrimeSubgraph(const NodeId id) {
    if (!has_max_prime_junction_tree__) computeMaxPrimeJunctionTree__();

    return node_2_max_prime_clique__[id];
  }


  /** @brief returns the Id of the clique created by the
   * elimination of a given node during the triangulation process */
  INLINE
  NodeId StaticTriangulation::createdJunctionTreeClique(const NodeId id) {
    return junction_tree_strategy_->createdClique(id);
  }


  /** @brief returns the Ids of the cliques of the junction tree created by the
   * elimination of the nodes */
  INLINE
  const NodeProperty< NodeId >& StaticTriangulation::createdJunctionTreeCliques() {
    return junction_tree_strategy_->createdCliques();
  }


  /// sets/unset the fill-ins storage in the standard triangulation procedure
  INLINE
  void StaticTriangulation::setFillIns(bool b) {
    we_want_fill_ins__ = b;
    elimination_sequence_strategy_->askFillIns(b);
  }


  /// returns the graph to be triangulated
  INLINE
  const UndiGraph* StaticTriangulation::originalGraph() const {
    return original_graph__;
  }


  /// returns the elimination sequence strategy used by the triangulation
  INLINE EliminationSequenceStrategy&
         StaticTriangulation::eliminationSequenceStrategy() const {
    return *elimination_sequence_strategy_;
  }


  /// returns the junction tree strategy used by the triangulation
  INLINE JunctionTreeStrategy& StaticTriangulation::junctionTreeStrategy() const {
    return *junction_tree_strategy_;
  }


  /// indicates wether minimality is required
  INLINE bool StaticTriangulation::isMinimalityRequired() const {
    return minimality_required__;
  }


} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
