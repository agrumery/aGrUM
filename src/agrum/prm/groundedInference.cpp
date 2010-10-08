/**************************************************************************
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
 * @brief Implementation of GroundedInference.
 *
 * @author Lionel TORTI
 *
 */
// ============================================================================
#include <agrum/prm/groundedInference.h>
// ============================================================================
#ifdef GUM_NO_INLINE
#include <agrum/prm/groundedInference.inl>
#endif // GUM_NO_INLINE
// ============================================================================
namespace gum {
namespace prm {

GroundedInference::~GroundedInference() {
  GUM_DESTRUCTOR( GroundedInference );
  if (__inf != 0) {
    delete __inf;
  }
  if (not __obs.empty()) {
    for (List<const Potential<prm_float>* >::iterator iter = __obs.begin(); iter != __obs.end(); ++iter) {
      // We used const ptrs only because of BayesNetInference::addEvidence() requires it
      delete const_cast<Potential<prm_float>*>(*iter);
    }
  }
}

void
GroundedInference::_evidenceAdded(const Chain& chain) {
  Potential<prm_float>* bn_obs = new Potential<prm_float>();
  // Retrieving the BN's variable
  std::stringstream var_name;
  var_name << chain.first->name() << "." << chain.second->safeName();
  bn_obs->add(__inf->bn().variableFromName(var_name.str()));
  // Retrievin the PRM's evidence and copying it in bn_obs
  const Potential<prm_float>* prm_obs = evidence(chain.first)[chain.second->id()];
  Instantiation i(*bn_obs), j(*prm_obs);
  for (i.setFirst(), j.setFirst(); not i.end(); i.inc(), j.inc()) {
    bn_obs->set(i, prm_obs->get(j));
  }
  __obs.insert(bn_obs);
}

void
GroundedInference::_evidenceRemoved(const Chain& chain) {
  std::stringstream var_name;
  var_name << chain.first->name() << "." << chain.second->safeName();
  const DiscreteVariable& var = __inf->bn().variableFromName(var_name.str());
  for (List<const Potential<prm_float>* >::iterator iter = __obs.begin(); iter != __obs.end(); ++iter) {
    if ((**iter).contains(var)) {
      __inf->eraseEvidence(*iter);
      const Potential<prm_float>* e = *iter;
      __obs.erase(iter);
      delete e;
      break;
    }
  }
}



} /* namespace prm */
} /* namespace gum */
// ============================================================================
