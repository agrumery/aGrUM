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

PRMInference::PRMInference(const PRM& prm, const Model& model):
 _prm(prm), _model(model)
{
  GUM_CONSTRUCTOR( PRMInference );
}

PRMInference::PRMInference(const PRM& prm, const std::string& model):
  _prm(prm), _model(prm.getModel(model))
{
  GUM_CONSTRUCTOR( PRMInference );
}

PRMInference::~PRMInference() {
  GUM_DESTRUCTOR( PRMInference );
  for (EvidenceIterator iter = __evidences.begin(); iter != __evidences.end(); ++iter) {
    delete *iter;
  }
  __evidences.clear();
}

void
PRMInference::clearEvidence() {
  for (EvidenceIterator iter = __evidences.begin(); iter != __evidences.end(); ++iter) {
    delete *iter;
  }
  __evidences.clear();
}


} /* namespace gum */
// ============================================================================
