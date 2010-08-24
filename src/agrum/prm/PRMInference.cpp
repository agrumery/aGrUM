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
 * @brief Implementation of PRMInference.
 *
 * @author Lionel TORTI
 *
 */
// ============================================================================
#include <agrum/prm/PRMInference.h>
// ============================================================================
#ifdef GUM_NO_INLINE
#include <agrum/prm/PRMInference.inl>
#endif // GUM_NO_INLINE
// ============================================================================
namespace gum {
namespace prm {

void
PRMInference::clearEvidence() {
  for (EvidenceIterator iter = __evidences.begin(); iter != __evidences.end(); ++iter) {
    for (PRMInference::EMapIterator jter = (*iter)->begin(); jter != (*iter)->end(); ++jter) {
      delete *jter;
    }
    delete *iter;
  }
  __evidences.clear();
}

PRMInference::PRMInference(const PRMInference& source):
  _prm(source._prm), _sys(source._sys)
{
  GUM_CONS_CPY( PRMInference );
  for (PRMInference::EvidenceConstIterator iter = source.__evidences.begin(); iter != source.__evidences.end(); ++iter) {
    __evidences.insert(iter.key(), new PRMInference::EMap());
    for (PRMInference::EMapIterator jter = (*iter)->begin(); jter != (*iter)->end(); ++jter) {
      Potential<prm_float>* e = new Potential<prm_float>();
      e->add(*((**jter).variablesSequence().front()));
      Instantiation i(*e);
      for (i.setFirst(); not i.end(); i.inc()) {
        e->set(i, (**jter).get(i));
      }
      __evidences[iter.key()]->insert(jter.key(), e);
    }
  }
}

PRMInference&
PRMInference::operator=(const PRMInference& source) {
  clearEvidence();
  _prm = source._prm;
  _sys = source._sys;
  for (PRMInference::EvidenceConstIterator iter = source.__evidences.begin(); iter != source.__evidences.end(); ++iter) {
    __evidences.insert(iter.key(), new PRMInference::EMap());
    for (PRMInference::EMapIterator jter = (*iter)->begin(); jter != (*iter)->end(); ++jter) {
      Potential<prm_float>* e = new Potential<prm_float>();
      e->add(*((**jter).variablesSequence().front()));
      Instantiation i(*e);
      for (i.setFirst(); not i.end(); i.inc()) {
        e->set(i, (**jter).get(i));
      }
      __evidences[iter.key()]->insert(jter.key(), e);
    }
  }
  return *this;
}

PRMInference::EMap&
PRMInference::__EMap(const Instance* i) {
  if (__evidences.exists(i)) {
    return *(__evidences[i]);
  } else {
    __evidences.insert(i, new PRMInference::EMap());
    return *(__evidences[i]);
  }
}

void
PRMInference::addEvidence(const Chain& chain, const Potential<prm_float>& p) {
  if (chain.first->exists(chain.second->id())) {
    if ( (p.nbrDim() != 1) or (not p.contains(chain.second->type().variable())) )
      GUM_ERROR(OperationNotAllowed, "illegal evidence for the given Attribute.");
    Potential<prm_float>* e = new Potential<prm_float>();
    e->add(chain.second->type().variable());
    Instantiation i(*e);
    for (i.setFirst(); not i.end(); i.inc())
      e->set(i, p.get(i));
    PRMInference::EMap& emap = __EMap(chain.first);
    if (emap.exists(chain.second->id())) {
      delete emap[chain.second->id()];
      emap[chain.second->id()] = e;
    } else {
      emap.insert(chain.second->id(), e);
    }
    _evidenceAdded(chain);
  } else {
    GUM_ERROR(NotFound, "the given Attribute does not belong to this Instance.");
  }
}

} /* namespace prm */
} /* namespace gum */
// ============================================================================
