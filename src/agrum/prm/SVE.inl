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
 * @brief Inline implementation of SVE.
 *
 * @author Lionel TORTI
 */
// ============================================================================
namespace gum {
namespace prm {

INLINE
SVE::SVE(const PRM& prm, const Model& model):
  PRMInference(prm, model), __class_elim_order(0)
{
  GUM_CONSTRUCTOR(SVE);
}

INLINE
SVE::SVE(const PRM& prm, const std::string& model):
  PRMInference(prm, model), __class_elim_order(0)
{
  GUM_CONSTRUCTOR(SVE);
}

INLINE
void
SVE::__insertEvidence(const Instance* i, BucketSet& pool) {
  for (PRMInference::EMapIterator iter = evidence(i).begin();
       iter != evidence(i).end(); ++iter) {
    pool.insert(const_cast<Potential<prm_float>*>(*iter));
  }
}

INLINE
std::vector<NodeId>&
SVE::__getElimOrder(const Class& c) {
  return *(__elim_orders[&c]);
}

INLINE
bool
SVE::__checkElimOrder(const Instance* first, const Instance* second) {
  if (__class_elim_order == 0) {
    __initElimOrder();
  }
  return (__class_elim_order->pos(&(first->type())) <=
          __class_elim_order->pos(&(second->type())));
}

INLINE
Potential<prm_float>*
SVE::__getAggPotential(const Instance* i, const Aggregate* agg) {
  return &(const_cast<Potential<prm_float>&>(i->get(agg->id()).cpf()));
}

INLINE
void
SVE::_evidenceAdded(const Instance* i, NodeId elt_id) {
  // Do nothing
}

INLINE
void
SVE::_evidenceRemoved(const Instance* i, NodeId elt_id) {
  // Do nothing
}

} /* namespace prm */
} /* namespace gum */
// ============================================================================
