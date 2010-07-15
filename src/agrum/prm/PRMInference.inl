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
PRMInference::~PRMInference() {
  GUM_DESTRUCTOR( PRMInference );
  clearEvidence();
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
PRMInference::marginal(const PRMInference::Chain& chain, Potential<prm_float>& m) {
  if (m.nbrDim() > 0) {
    GUM_ERROR(OperationNotAllowed, "the given Potential is not empty.");
  }
  m.add(chain.second->type().variable());
  _marginal(chain, m);
}

INLINE
void
PRMInference::joint(const std::vector< PRMInference::Chain >& chains, Potential<prm_float>& j) {
  if (j.nbrDim() > 0) {
    GUM_ERROR(OperationNotAllowed, "the given Potential is not empty.");
  }
  for (std::vector< PRMInference::Chain >::const_iterator chain = chains.begin(); chain != chains.end(); ++chain) {
    j.add(chain->second->type().variable());
  }
  _joint(chains, j);
}

} /* namespace prm */
} /* namespace gum */
// ============================================================================
