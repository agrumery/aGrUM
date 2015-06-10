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
 * @brief Implementation of gspan.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#include <agrum/PRM/inference/gspan.h>

namespace gum {
  namespace prm {

    template class GSpan<double>;
    // double
    // GSpan::patterns_cost() const {
    //  double cost = 0;
    //  typedef HashTable<gspan::Pattern*, MatchedInstances*>::const_iterator
    //  MatchIter;
    //  for (MatchIter match = __matched_instances.begin(); match !=
    //  __matched_instances.end(); ++match) {
    //    cost += (tree().frequency(*(match.key())) * tree().cost(*(match.key())));
    //  }
    //  for (UndiGraph::NodeIterator node = __graph->graph().beginNodes(); node !=
    //  __graph->graph().endNodes(); ++node) {
    //    if (not __chosen.exists(__graph->node(*node).n)) {
    //      cost += __instance_cost(__graph->node(*node).n);
    //    }
    //  }
    //  return cost;
    //}
    //
    // double
    // GSpan::sve_cost() const {
    //  unsigned long cost = 0;
    //  for (UndiGraph::NodeIterator node = __graph->graph().beginNodes(); node !=
    //  __graph->graph().endNodes(); ++node) {
    //    cost += __instance_cost(__graph->node(*node).n);
    //  }
    //  return cost;
    //}
    //
    // double
    // GSpan::__instance_cost(Instance* i) const {
    //  unsigned cost = 1;
    //  Set<ClassElement*> inputs;
    //  for (Set<SlotChain*>::iterator slot_chain = i->type().slotChains().begin();
    //  slot_chain != i->type().slotChains().end(); ++slot_chain) {
    //    for (Set<Instance*>::iterator instance =
    //    i->getInstances((*slot_chain)->id()).begin();
    //         instance != i->getInstances((*slot_chain)->id()).end(); ++instance) {
    //      if (not
    //      inputs.exists(&((*instance)->get((*slot_chain)->lastElt().id())))) {
    //        inputs.insert(&((*instance)->get((*slot_chain)->lastElt().id())));
    //        cost *=
    //        (*instance)->get((*slot_chain)->lastElt().id()).type().variable().domainSize();
    //      }
    //    }
    //  }
    //  for (DAG::NodeIterator node = i->type().dag().beginNodes(); node !=
    //  i->type().dag().endNodes(); ++node) {
    //    if (i->type().isOutputNode(i->type().get(*node)) and i->hasRefAttr(*node))
    //    {
    //      cost *= i->get(*node).type().variable().domainSize();
    //    }
    //  }
    //  return cost;
    //}

  } /* namespace prm */
} /* namespace gum */
