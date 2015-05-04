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
 * @brief source implementations for computing default triangulations of graphs
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/config.h>
#include <agrum/graphs/triangulations/defaultTriangulation.h>
#include <agrum/graphs/eliminations/defaultEliminationSequenceStrategy.h>
#include <agrum/graphs/defaultJunctionTreeStrategy.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  /* =========================================================================== */
  /* =========================================================================== */
  /* ===            DEFAULT TRIANGULATION ALGORITHM USED BY AGRUM            === */
  /* =========================================================================== */
  /* =========================================================================== */

  /// basic constructor. initialize the triangulation

  DefaultTriangulation::DefaultTriangulation(const UndiGraph *theGraph,
                                             const NodeProperty<Size> *modal,
                                             bool minimality, float theRatio,
                                             float theThreshold)
      : UnconstrainedTriangulation(theGraph, modal,
                                   DefaultEliminationSequenceStrategy(),
                                   DefaultJunctionTreeStrategy(), minimality),
        __quasi_ratio(theRatio), __threshold(theThreshold) {
    // for debugging purposes
    GUM_CONSTRUCTOR(DefaultTriangulation);
  }

  /// default constructor: initialize the triangulation for an empty graph

  DefaultTriangulation::DefaultTriangulation(bool minimality, double theRatio,
                                             double theThreshold)
      : UnconstrainedTriangulation(DefaultEliminationSequenceStrategy(),
                                   DefaultJunctionTreeStrategy(), minimality),
        __quasi_ratio(theRatio), __threshold(theThreshold) {
    // for debugging purposes
    GUM_CONSTRUCTOR(DefaultTriangulation);
  }

  /*

  /// copy constructor

  DefaultTriangulation::DefaultTriangulation
  ( const DefaultTriangulation& from ) :
    UnconstrainedTriangulation (


    static_cast<const
  UnconstrainedEliminationSequenceStrategy&>(*(from._elimination_sequence_strategy))
  ),
    __original_graph( from.__original_graph ),
    __triangulated_graph( from.__triangulated_graph ),
    __modalities (from.__modalities),
    __fill_ins( from.__fill_ins ),
    __elim_order( from.__elim_order ),
    __reverse_elim_order( from.__reverse_elim_order ),
    __elim_cliques( from.__elim_cliques ), __elim_tree( from.__elim_tree ),
    __junction_tree( from.__junction_tree ),
    __node_2_junction_clique( from.__node_2_junction_clique ),
    __max_prime_junction_tree( from.__max_prime_junction_tree ),
    __node_2_max_prime_clique( from.__node_2_max_prime_clique ),
    __has_triangulation( from.__has_triangulation ),
    __has_triangulated_graph( from.__has_triangulated_graph ),
    __has_elimination_tree( from.__has_elimination_tree ),
    __has_junction_tree( from.__has_junction_tree ),
    __has_max_prime_junction_tree( from.__has_max_prime_junction_tree ),
    __quasi_ratio( from.__quasi_ratio ), __threshold( from.__threshold ),
    __minimality_required( from.__minimality_required ),
    __added_fill_ins( from.__added_fill_ins ),
    __we_want_fill_ins( from.__we_want_fill_ins ) {
    // for debugging purposes
    GUM_CONS_CPY( DefaultTriangulation );
  }
  */

  /// destructor

  DefaultTriangulation::~DefaultTriangulation() {
    // for debugging purposes
    GUM_DESTRUCTOR(DefaultTriangulation);
  }

  /// virtual copy constructor

  DefaultTriangulation *DefaultTriangulation::newFactory() const {
    return new DefaultTriangulation;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
