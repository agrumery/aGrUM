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


  /* ============================================================================ */
  /* ============================================================================ */
  /* ===            CLASS FOR COMPUTING INCREMENTAL TRIANGULATIONS            === */
  /* ============================================================================ */
  /* ============================================================================ */


  // ================================================================================
  /// returns a junction tree corresponding to the current graph
  // ================================================================================
  INLINE
  const CliqueGraph& IncrementalTriangulation::junctionTree() {
    updateTriangulation();
    return __junction_tree;
  }


  // ================================================================================
  /// returns the junction tree of the maximal prime subgraphs
  // ================================================================================
  INLINE
  const CliqueGraph&
  IncrementalTriangulation::maxPrimeSubgraphTree() {
    updateTriangulation();
    return __T_mpd;
  }


  // ================================================================================
  /// returns the triangulation algorithm (useful for fine tuning it)
  // ================================================================================
  INLINE const Triangulation&
  IncrementalTriangulation::getTriangulationAlgo() const  {
    return *__triangulation;
  }


  // ================================================================================
  /// returns a clique containing a given node of the triangulated graph
  // ================================================================================
  INLINE NodeId
  IncrementalTriangulation::getContainerClique ( NodeId id ) const {
    const std::vector<NodeId>& contain_cliques =
      __cliques_of_mps[__mps_of_node[id][0]];

    for ( unsigned int i = 0; i < contain_cliques.size(); ++i )
      if ( __junction_tree.clique(contain_cliques[i]).contains( id ) )
        return contain_cliques[i];

    GUM_ERROR( NotFound, "no container clique" );
  }


} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
