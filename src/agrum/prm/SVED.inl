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
 * @brief Inline implementation of SVED.
 *
 * @author Lionel TORTI
 */
// ============================================================================
namespace gum {
namespace prm {

INLINE
SVED::SVED(const PRM& prm, const System& model):
  PRMInference(prm, model), __class_elim_order(0), __bb(*this)
{
  GUM_CONSTRUCTOR(SVED);
}

INLINE
SVED::SVED(const PRM& prm, const std::string& model):
  PRMInference(prm, model), __class_elim_order(0), __bb(*this)
{
  GUM_CONSTRUCTOR(SVED);
}

INLINE
void
SVED::__insertEvidence(const Instance* i, BucketSet& pool) {
  for (PRMInference::EMapIterator iter = evidence(i).begin(); iter != evidence(i).end(); ++iter) {
    pool.insert(const_cast<Potential<prm_float>*>(*iter));
  }
}

INLINE
std::vector<NodeId>&
SVED::__getElimOrder(const Class& c) {
  return *(__elim_orders[&c]);
}

INLINE
bool
SVED::__checkElimOrder(const Instance* first, const Instance* second) {
  if (__class_elim_order == 0) {
    __initElimOrder();
  }
  return (__class_elim_order->pos(&(first->type())) <= __class_elim_order->pos(&(second->type())));
}

INLINE
Potential<prm_float>*
SVED::__getAggPotential(const Instance* i, const Aggregate* agg) {
  return &(const_cast<Potential<prm_float>&>(i->get(agg->safeName()).cpf()));
}

INLINE
void
SVED::_evidenceAdded(const SVED::Chain& chain) {
  // Do nothing
}

INLINE
void
SVED::_evidenceRemoved(const SVED::Chain& chain) {
  // Do nothing
}

INLINE
Set<NodeId>&
SVED::__getAttrSet(const Instance* i) {
  try {
    return *(__req_set[&(__bb.requisiteNodes(i))].first);
  } catch (NotFound&) {
    __initReqSets(i);
    return *(__req_set[&(__bb.requisiteNodes(i))].first);
  }
}

INLINE
Set<NodeId>&
SVED::__getSCSet(const Instance* i) {
  try {
    return *(__req_set[&(__bb.requisiteNodes(i))].second);
  } catch (NotFound&) {
    __initReqSets(i);
    return *(__req_set[&(__bb.requisiteNodes(i))].second);
  }
}

INLINE
void
SVED::__callDownwardElimination(const Instance* i, NodeId attr,
                                BucketSet& pool, BucketSet& trash,
                                List<const Instance*>& elim_list,
                                Set<const Instance*>& ignore)
{
  // try {
  //   typedef Set< Instance::InverseSC* >::iterator InvIter;
  //   for (InvIter inv_sc = i->inverseSC(attr).begin(); inv_sc != i->inverseSC(attr).end(); ++inv_sc) {
  //     if ( (not ignore.exists((*inv_sc)->first)) and (__bb.exists((*inv_sc)->first)) ) {
  //       __eliminateNodesDownward(i, (*inv_sc)->first, pool, trash, elim_list, ignore);
  //     }
  //   }
  // } catch (NotFound&) {
  //   // Ok
  // }
}

INLINE
void
SVED::__reduceElimList(const Instance* i, List<const Instance*>& elim_list,
                       List<const Instance*>& reduced_list, Set<const Instance*>& ignore,
                       BucketSet& pool, BucketSet& trash)
{
  while (not elim_list.empty()) {
    if (__checkElimOrder(i, elim_list.front())) {
      if ( (not ignore.exists(elim_list.front())) and (__bb.exists(elim_list.front())) ) {
        __eliminateNodesDownward(i, elim_list.front(), pool, trash, elim_list, ignore);
      }
    } else if (__bb.exists(elim_list.front())) {
      reduced_list.insert(elim_list.front());
    }
    elim_list.popFront();
  }
}


} /* namespace prm */
} /* namespace gum */
// ============================================================================
