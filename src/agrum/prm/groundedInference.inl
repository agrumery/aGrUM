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
 * @brief Inline implementation of GroundedInference.
 *
 * @author Lionel TORTI
 *
 */
// ============================================================================
namespace gum {
namespace prm {

INLINE
GroundedInference::GroundedInference(const PRM& prm, const System& system):
  PRMInference(prm, system), __inf(0)
{
  GUM_CONSTRUCTOR( GroundedInference );
}

INLINE
GroundedInference::GroundedInference(const GroundedInference& source):
  PRMInference(source), __inf(0)
{
  GUM_CONS_CPY(GroundedInference);
  GUM_ERROR(FatalError, "illegal to copy constructor");
}

INLINE
GroundedInference::~GroundedInference() {
  GUM_DESTRUCTOR( GroundedInference );
  if (__inf != 0) {
    delete __inf;
  }
}

INLINE
GroundedInference&
GroundedInference::operator=(const GroundedInference& source) {
  GUM_ERROR(FatalError, "illegal call to copy operator");
}

INLINE
BayesNetInference<prm_float>&
GroundedInference::getBNInference() {
  if (__inf != 0) {
    return *__inf;
  } else {
    GUM_ERROR(NotFound, "the inference engine is not yet defined");
  }
}

INLINE
void
GroundedInference::setBNInference(BayesNetInference<prm_float>* bn_inf) {
  if (__inf != 0) {
    delete __inf;
  }
  __inf = bn_inf;
}

INLINE
void
GroundedInference::_evidenceAdded(const Chain& chain) {
  Potential<prm_float>* e = new Potential<prm_float>(*(evidence(chain.first)[chain.second->id()]));
  __obs.insert(e);
}

INLINE
void
GroundedInference::_evidenceRemoved(const Chain& chain) {
  for (List< Potential<prm_float>* >::iterator iter = __obs.begin(); iter != __obs.end(); ++iter) {
    if ((**iter).contains(chain.second->type().variable())) {
      __inf->eraseEvidence(*iter);
      __obs.erase(iter);
      delete *iter;
      break;
    }
  }
}

INLINE
void
GroundedInference::_marginal(const Chain& chain, Potential<prm_float>& m) {
  if (__inf == 0) {
    GUM_ERROR(OperationNotAllowed, "no inference engine defined");
  }
  std::stringstream sBuff;
  sBuff << chain.first->name() << "." << chain.second->safeName();
  m = __inf->marginal(__inf->bn().idFromName(sBuff.str()));
}

INLINE
void
GroundedInference::_joint(const std::vector< Chain >& queries, Potential<prm_float>& j) {
  GUM_ERROR(FatalError, "not yet implemented");
}

} /* namespace prm */
} /* namespace gum */
// ============================================================================
