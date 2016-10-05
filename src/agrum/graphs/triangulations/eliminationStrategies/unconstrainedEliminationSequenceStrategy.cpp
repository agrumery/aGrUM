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
 * @brief Base Class for all elimination sequence algorithms that require only
 *the
 * graph to be triangulated and the nodes domain sizes to produce the node
 * elimination ordering.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/graphs/triangulations/eliminationStrategies/unconstrainedEliminationSequenceStrategy.h>

namespace gum {

  /// default constructor
  UnconstrainedEliminationSequenceStrategy::
      UnconstrainedEliminationSequenceStrategy() {
    // for debugging purposes
    GUM_CONSTRUCTOR( UnconstrainedEliminationSequenceStrategy );
  }

  /// constructor for an a priori non empty graph
  UnconstrainedEliminationSequenceStrategy::
      UnconstrainedEliminationSequenceStrategy(
          UndiGraph* graph, const NodeProperty<Size>* dom_sizes )
      : EliminationSequenceStrategy( graph, dom_sizes ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( UnconstrainedEliminationSequenceStrategy );
  }

  /// copy constructor
  UnconstrainedEliminationSequenceStrategy::
      UnconstrainedEliminationSequenceStrategy(
          const UnconstrainedEliminationSequenceStrategy& from )
      : EliminationSequenceStrategy( from ) {
    // for debugging purposes
    GUM_CONS_CPY( UnconstrainedEliminationSequenceStrategy );
  }

  /// move constructor
  UnconstrainedEliminationSequenceStrategy::
      UnconstrainedEliminationSequenceStrategy(
          UnconstrainedEliminationSequenceStrategy&& from )
      : EliminationSequenceStrategy( std::move( from ) ) {
    // for debugging purposes
    GUM_CONS_MOV( UnconstrainedEliminationSequenceStrategy );
  }

  /// destructor
  UnconstrainedEliminationSequenceStrategy::
      ~UnconstrainedEliminationSequenceStrategy() {
    // for debugging purposes
    GUM_DESTRUCTOR( UnconstrainedEliminationSequenceStrategy );
  }

} /* namespace gum */
