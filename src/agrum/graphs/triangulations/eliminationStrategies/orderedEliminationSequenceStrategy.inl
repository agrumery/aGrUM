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
 * @brief An Elimination sequence algorithm that imposes a given complete
 * ordering on the nodes elimination sequence
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

namespace gum {


  // returns the current complete ordering
  INLINE const std::vector<NodeId>*
               OrderedEliminationSequenceStrategy::order() const noexcept {
    return __order;
  }


  // indicates whether a new complete ordering is needed
  INLINE bool OrderedEliminationSequenceStrategy::isOrderNeeded() const noexcept {
    return __order_needed;
  }


} /* namespace gum */
