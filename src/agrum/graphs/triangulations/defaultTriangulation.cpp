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
#include <agrum/graphs/triangulations/eliminationStrategies/defaultEliminationSequenceStrategy.h>
#include <agrum/graphs/triangulations/junctionTreeStrategies/defaultJunctionTreeStrategy.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  /// basic constructor. initialize the triangulation
  DefaultTriangulation::DefaultTriangulation( const UndiGraph*          theGraph,
                                              const NodeProperty<Size>* domsizes,
                                              bool                      minimality,
                                              double                    theRatio,
                                              double theThreshold )
      : UnconstrainedTriangulation( theGraph,
                                    domsizes,
                                    DefaultEliminationSequenceStrategy(),
                                    DefaultJunctionTreeStrategy(),
                                    minimality )
      , __quasi_ratio( theRatio )
      , __threshold( theThreshold ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( DefaultTriangulation );
  }

  /// default constructor: initialize the triangulation for an empty graph
  DefaultTriangulation::DefaultTriangulation( bool   minimality,
                                              double theRatio,
                                              double theThreshold )
      : UnconstrainedTriangulation( DefaultEliminationSequenceStrategy(),
                                    DefaultJunctionTreeStrategy(),
                                    minimality )
      , __quasi_ratio( theRatio )
      , __threshold( theThreshold ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( DefaultTriangulation );
  }

  /// copy constructor
  DefaultTriangulation::DefaultTriangulation( const DefaultTriangulation& from )
      : UnconstrainedTriangulation( from )
      , __quasi_ratio( from.__quasi_ratio )
      , __threshold( from.__threshold ) {
    // for debugging purposes
    GUM_CONS_CPY( DefaultTriangulation );
  }

  /// move constructor
  DefaultTriangulation::DefaultTriangulation( DefaultTriangulation&& from )
      : UnconstrainedTriangulation( std::move( from ) )
      , __quasi_ratio( from.__quasi_ratio )
      , __threshold( from.__threshold ) {
    // for debugging purposes
    GUM_CONS_MOV( DefaultTriangulation );
  }

  /// destructor
  DefaultTriangulation::~DefaultTriangulation() {
    // for debugging purposes
    GUM_DESTRUCTOR( DefaultTriangulation );
  }

  /// virtual clone constructor
  DefaultTriangulation* DefaultTriangulation::newFactory() const {
    return new DefaultTriangulation(
        isMinimalityRequired(), __quasi_ratio, __threshold );
  }

  /// virtual copy constructor
  DefaultTriangulation* DefaultTriangulation::copyFactory() const {
    return new DefaultTriangulation( *this );
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
