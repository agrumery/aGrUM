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
 * @brief implementation of the base class for all elimination sequence algorithms
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/config.h>

#ifndef NDEBUG
#include <agrum/core/debug.h>
#endif

#include <agrum/graphs/eliminations/eliminationSequenceStrategy.h>

namespace gum {

  // an empty fill-ins set returned by default when we ask for a fill-ins set
  const EdgeSet& EliminationSequenceStrategy::__empty_fill_ins() {
    static bool first_use = true;
    if (first_use) {
      first_use = false;
#ifndef NDEBUG
      __debug__::__dec_creation("Set", "__empty_edge_set", 0, "static variable correction", 0);
      __debug__::__dec_creation("HashTable", "__empty_edge_set", 0,
          "static variable correction", 0);
#endif
    }
    static EdgeSet empty_fill_ins;
    return empty_fill_ins;
  }


  /// default constructor
  EliminationSequenceStrategy::EliminationSequenceStrategy() {
    // for debugging purposes
    GUM_CONSTRUCTOR(EliminationSequenceStrategy);
  }

  /// copy constructor
  EliminationSequenceStrategy::EliminationSequenceStrategy(
      const EliminationSequenceStrategy &) {
    // for debugging purposes
    GUM_CONS_CPY(EliminationSequenceStrategy);
  }

  /// destructor
  EliminationSequenceStrategy::~EliminationSequenceStrategy() {
    // for debugging purposes
    GUM_DESTRUCTOR(EliminationSequenceStrategy);
  }

  /// performs all the graph/fill-ins updates provided
  void EliminationSequenceStrategy::eliminationUpdate(const NodeId node) {}

  /** @brief in case fill-ins are provided, this function returns the fill-ins
   * due to all the nodes eliminated so far */
  const EdgeSet &EliminationSequenceStrategy::fillIns() { return __empty_fill_ins(); }

} /* namespace gum */
