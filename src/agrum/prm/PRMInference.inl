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
 * @brief Inline implementation of PRMInference.
 *
 * @author Lionel TORTI
 *
 */
// ============================================================================
namespace gum {
namespace prm {

INLINE
PRMInference::PRMInference(const PRM& prm, const System& system):
 _prm(&prm), _sys(&system)
{
  GUM_CONSTRUCTOR( PRMInference );
}

INLINE
PRMInference::PRMInference(const PRMInference& source):
  _prm(source._prm), _sys(source._sys)
{
  GUM_CONS_CPY( PRMInference );
  for (PRMInference::EvidenceIterator iter = source.__evidences.begin(); iter != source.__evidences.end(); ++iter) {
    __evidences.insert(iter.key(), new PRMInference::EMap());
    for (PRMInference::EMapIterator jter = (*iter)->begin(); jter != (*iter)->end(); ++jter) {
      __evidences[iter.key()]->insert(jter.key(), new Potential<prm_float>(**jter));
    }
  }
}

INLINE
PRMInference::~PRMInference() {
  GUM_DESTRUCTOR( PRMInference );
  clearEvidence();
}

INLINE
PRMInference&
PRMInference::operator=(const PRMInference& source) {
  clearEvidence();
  _prm = source._prm;
  _sys = source._sys;
  for (PRMInference::EvidenceIterator iter = source.__evidences.begin(); iter != source.__evidences.end(); ++iter) {
    __evidences.insert(iter.key(), new PRMInference::EMap());
    for (PRMInference::EMapIterator jter = (*iter)->begin(); jter != (*iter)->end(); ++jter) {
      __evidences[iter.key()]->insert(jter.key(), new Potential<prm_float>(**jter));
    }
  }
  return *this;
}

INLINE
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

INLINE
PRMInference::EMap&
PRMInference::evidence(const Instance& i) {
  try {
    return *(__evidences[&i]);
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "this instance has no evidence.");
  }
}

INLINE
const PRMInference::EMap&
PRMInference::evidence(const Instance& i) const {
  try {
    return *(__evidences[&i]);
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "this instance has no evidence.");
  }
}

INLINE
PRMInference::EMap&
PRMInference::evidence(const Instance* i) {
  try {
    return *(__evidences[i]);
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "this instance has no evidence.");
  }
}

INLINE
const PRMInference::EMap&
PRMInference::evidence(const Instance* i) const {
  try {
    return *(__evidences[i]);
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "this instance has no evidence.");
  }
}

INLINE
bool
PRMInference::hasEvidence(const Instance& i) const {
  return __evidences.exists(&i);
}

INLINE
bool
PRMInference::hasEvidence(const Instance* i) const {
  return __evidences.exists(i);
}

INLINE
bool
PRMInference::hasEvidence(const Chain& chain) const {
  return (hasEvidence(chain.first))?evidence(chain.first).exists(chain.second->id()):false;
}

INLINE
void
PRMInference::addEvidence(const Chain& chain, const Potential<prm_float>& p) {
  if (chain.first->exists(chain.second->id())) {
    __addEvidence(chain, p);
  } else {
    GUM_ERROR(NotFound, "the given Attribute does not belong to this Instance.");
  }
}

INLINE
void
PRMInference::removeEvidence(const Chain& chain) {
  if (chain.first->exists(chain.second->id())) {
    __removeEvidence(chain);
  } else {
    GUM_ERROR(NotFound, "the given Attribute does not belong to this Instance.");
  }
}

INLINE
void
PRMInference::__addEvidence(const Chain& chain, const Potential<prm_float>& p) {
  if ( (p.nbrDim() != 1) and (p.contains(chain.second->type().variable())) ) {
    GUM_ERROR(OperationNotAllowed, "illegal evidence for the given Attribute.");
  }
  try {
    __EMap(chain.first)[chain.second->id()] = new Potential<prm_float>(p);
  } catch (NotFound&) {
    __EMap(chain.first).insert(chain.second->id(), new Potential<prm_float>(p));
  }
  _evidenceAdded(chain);
}

INLINE
void
PRMInference::__removeEvidence(const Chain& chain) {
  if (__EMap(chain.first).exists(chain.second->id())) {
    _evidenceRemoved(chain);
    delete __EMap(chain.first)[chain.second->id()];
    __EMap(chain.first).erase(chain.second->id());
  }
}

INLINE
PRMInference::EMap&
PRMInference::__EMap(const Instance* i) {
  try {
    return *(__evidences[i]);
  } catch (NotFound&) {
    __evidences.insert(i, new PRMInference::EMap());
    return *(__evidences[i]);
  }
}

INLINE
void
PRMInference::marginal(const std::string& chain, Potential<prm_float>& m) {
  if (m.nbrDim() > 0) {
    GUM_ERROR(OperationNotAllowed, "the given Potential is not empty.");
  }
  try {
    size_t idx = chain.find(".");
    const Instance& i = _sys->get(chain.substr(0, idx));
    const Attribute& elt = i.get(chain.substr(idx + 1));
    m.add(elt.type().variable());
    _marginal(std::make_pair(&i, &elt), m);
  } catch (NotFound&) {
    GUM_ERROR(WrongType, "the given element is not an Attribute.");
  }
}

INLINE
void
PRMInference::joint(const Set<std::string> chains, Potential<prm_float>& j) {
  std::vector< Chain > queries;
  try {
    for (Set<std::string>::const_iterator chain = chains.begin(); chain != chains.end(); ++chain) {
      size_t idx = chain->find(".");
      const Instance& i = _sys->get(chain->substr(0, idx));
      const Attribute& elt = i.get(chain->substr(idx + 1));
      j.add(elt.type().variable());
      queries.push_back(std::make_pair(&i, &elt));
    }
  } catch (NotFound&) {
    GUM_ERROR(WrongType, "the given element is not an Attribute.");
  }
  _joint(queries, j);
}

} /* namespace prm */
} /* namespace gum */
// ============================================================================
