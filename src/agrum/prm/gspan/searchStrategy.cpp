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
/**
 * @file
 * @brief Implementation of the DFSTree class.
 *
 * @author Lionel TORTI
 */
// ============================================================================
#include <agrum/prm/gspan/DFSTree.h>
// ============================================================================
#ifdef GUM_NO_INLINE
#include <agrum/prm/gspan/searchStrategy.inl>
#endif

namespace gum {
namespace prm {
namespace gspan {

double
SearchStrategy::_computeCost(const Pattern& p) {
  double cost = 0;
  const Sequence<Instance*>& seq = **(_tree->data(p).iso_map.begin());
  const Set<SlotChain*>* chains = 0;
  const Set<Instance*>* instances = 0;
  Sequence<ClassElement*> input_set;
  for (Sequence<Instance*>::iterator iter = seq.begin(); iter != seq.end(); ++iter) {
    chains = &((**iter).type().slotChains());
    for (Set<SlotChain*>::iterator input = chains->begin(); input != chains->end(); ++input) {
      instances = &((**iter).getInstances((**input).id()));
      for (Set<Instance*>::iterator jter = instances->begin(); jter != instances->end(); ++jter) {
        if ((not seq.exists(*jter)) and (not input_set.exists(&((*jter)->get((*input)->lastElt().safeName())))) ) {
          cost += std::log((*input)->type().variable().domainSize());
          input_set.insert(&((*jter)->get((*input)->lastElt().safeName())));
        }
      }
    }
    for (Instance::InvRefIterator vec = (**iter).beginInvRef(); vec != (**iter).endInvRef(); ++vec) {
      for (std::vector< std::pair<Instance*, std::string> >::iterator inverse = (**vec).begin(); inverse != (**vec).end(); ++inverse) {
        if (not seq.exists(inverse->first)) {
          cost += std::log((*iter)->get(vec.key()).type().variable().domainSize());
          break;
        }
      }
    }
  }
  return cost;
}

} /* namespace gspan */
} /* namespace prm */
} /* namespace gum */
// ============================================================================
