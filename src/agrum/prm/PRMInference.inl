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
PRMInference::hasEvidence(const Instance& i, const Attribute& a) const {
  return (hasEvidence(i))?evidence(i).exists(a.id()):false;
}

INLINE
bool
PRMInference::hasEvidence(const Instance* i, const Attribute* a) const {
  return (hasEvidence(i))?evidence(i).exists(a->id()):false;
}

INLINE
void
PRMInference::addEvidence(const std::string& inst_name, const std::string& elt_name, const Potential<prm_float>& p) {
  if (_model.get(inst_name)[elt_name].elt_type() == ClassElement::prm_attribute) {
    __addEvidence(&(_model.get(inst_name)), static_cast<const Attribute*>(&(_model.get(inst_name)[elt_name])), p);
  } else {
    GUM_ERROR(WrongType, "the given element is not an Attribute.");
  }
}

INLINE
void
PRMInference::addEvidence(const std::string& slot_chain, const Potential<prm_float>& p) {
  std::size_t idx = slot_chain.find(".");
  const Instance* i = &(_model.get(slot_chain.substr(0, idx)));
  const ClassElement& elt = i->get(slot_chain.substr(idx + 1, std::string::npos));
  if (elt.elt_type() == ClassElement::prm_attribute) {
    __addEvidence(i, static_cast<const Attribute*>(&elt), p);
  } else {
    GUM_ERROR(WrongType, "the given element is not an Attribute.");
  }
}

INLINE
void
PRMInference::addEvidence(const Instance& i, const Attribute& elt, const Potential<prm_float>& p) {
  if (i.exists(elt.id())) {
    __addEvidence(&i, &elt, p);
  } else {
    GUM_ERROR(NotFound, "the given Attribute does not belong to this Instance.");
  }
}

INLINE
void
PRMInference::addEvidence(const Instance& i, NodeId elt_id, const Potential<prm_float>& p) {
  if (i[elt_id].elt_type() == ClassElement::prm_attribute) {
    __addEvidence(&i, static_cast<const Attribute*>(&(i[elt_id])), p);
  } else {
    GUM_ERROR(WrongType, "the given element is not an Attribute.");
  }
}

INLINE
void
PRMInference::removeEvidence(const std::string& inst_name, const std::string& elt_name) {
  if (_model.get(inst_name)[elt_name].elt_type() == ClassElement::prm_attribute) {
    __removeEvidence(&(_model.get(inst_name)), static_cast<const Attribute*>(&(_model.get(inst_name)[elt_name])));
  } else {
    GUM_ERROR(WrongType, "the given element is not an Attribute.");
  }
}

INLINE
void
PRMInference::removeEvidence(const std::string& slot_chain) {
  std::size_t idx = slot_chain.find(".");
  const Instance* i = &(_model.get(slot_chain.substr(0, idx)));
  const ClassElement& elt = i->get(slot_chain.substr(idx + 1, std::string::npos));
  if (elt.elt_type() == ClassElement::prm_attribute) {
    __removeEvidence(i, static_cast<const Attribute*>(&elt));
  } else {
    GUM_ERROR(WrongType, "the given element is not an Attribute.");
  }
}

INLINE
void
PRMInference::removeEvidence(const Instance& i, const Attribute& elt) {
  if (i.exists(elt.id())) {
    __removeEvidence(&i, &elt);
  } else {
    GUM_ERROR(NotFound, "the given Attribute does not belong to this Instance.");
  }
}

INLINE
void
PRMInference::removeEvidence(const Instance& i, NodeId elt_id) {
  if (i[elt_id].elt_type() == ClassElement::prm_attribute) {
    __removeEvidence(&i, static_cast<const Attribute*>(&(i[elt_id])));
  } else {
    GUM_ERROR(WrongType, "the given element is not an Attribute.");
  }
}

INLINE
void
PRMInference::__addEvidence(const Instance* i, const Attribute* elt,
                            const Potential<prm_float>& p) {
  if ( (p.nbrDim() != 1) and (p.contains(elt->type().variable())) ) {
    GUM_ERROR(OperationNotAllowed, "illegal evidence for the given Attribute.");
  }
  try {
    __EMap(i)[elt->id()] = &p;
  } catch (NotFound&) {
    __EMap(i).insert(elt->id(), &p);
  }
  _evidenceAdded(i, elt->id());
}

INLINE
void
PRMInference::__removeEvidence(const Instance* i, const Attribute* elt) {
  _evidenceRemoved(i, elt->id());
  __EMap(i).erase(elt->id());
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
  std::size_t idx = chain.find(".");
  const Instance* i = &(_model.get(chain.substr(0, idx)));
  const ClassElement& elt = i->get(chain.substr(idx + 1, std::string::npos));
  if (elt.elt_type() == ClassElement::prm_attribute) {
    m.add(i->get(elt.id()).type().variable());
    _marginal(i, static_cast<const Attribute*>(&elt), m);
  } else {
    GUM_ERROR(WrongType, "the given element is not an Attribute.");
  }
}

INLINE
void
PRMInference::joint(const Set<std::string> chains, Potential<prm_float>& j) {
  GUM_ERROR(FatalError, "not implemented yet.");
}


} /* namespace prm */
} /* namespace gum */
// ============================================================================
