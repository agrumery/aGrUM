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
 * @brief base class for graph triangulations without constraints on nodes
 * elimination ordering.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/config.h>
#include <agrum/graphs/algorithms/triangulations/eliminationStrategies/unconstrainedEliminationSequenceStrategy.h>
#include <agrum/graphs/algorithms/triangulations/unconstrainedTriangulation.h>

namespace gum {

  // default constructor
  UnconstrainedTriangulation::UnconstrainedTriangulation(
      const UnconstrainedEliminationSequenceStrategy& elimSeq,
      const JunctionTreeStrategy&                     JTStrategy,
      bool                                            minimality )
      : StaticTriangulation( elimSeq, JTStrategy, minimality ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( UnconstrainedTriangulation );
  }

  // constructor with a given graph
  UnconstrainedTriangulation::UnconstrainedTriangulation(
      const UndiGraph*                                theGraph,
      const NodeProperty<Size>*                       domsizes,
      const UnconstrainedEliminationSequenceStrategy& elimSeq,
      const JunctionTreeStrategy&                     JTStrategy,
      bool                                            minimality )
      : StaticTriangulation(
            theGraph, domsizes, elimSeq, JTStrategy, minimality ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( UnconstrainedTriangulation );
  }

  /// copy constructor
  UnconstrainedTriangulation::UnconstrainedTriangulation(
      const UnconstrainedTriangulation& from )
      : StaticTriangulation( from ) {
    // for debugging purposes
    GUM_CONS_CPY( UnconstrainedTriangulation );
  }

  /// move constructor
  UnconstrainedTriangulation::UnconstrainedTriangulation(
      UnconstrainedTriangulation&& from )
      : StaticTriangulation( std::move( from ) ) {
    // for debugging purposes
    GUM_CONS_MOV( UnconstrainedTriangulation );
  }

  /// destructor
  UnconstrainedTriangulation::~UnconstrainedTriangulation() {
    // for debugging purposes
    GUM_DESTRUCTOR( UnconstrainedTriangulation );
  }

} /* namespace gum */