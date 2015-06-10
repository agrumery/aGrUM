/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
 * @brief Implementation of the non pure virtual methods of
 * BayesNetInference.
 */

// to ease parsers in IDE
#include <agrum/BN/inference/BayesNetInference.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  // Default Constructor
  template <typename GUM_SCALAR>
  BayesNetInference<GUM_SCALAR>::BayesNetInference(const IBayesNet<GUM_SCALAR> &bn)
      : __bayesNet(bn) {
    GUM_CONSTRUCTOR(BayesNetInference);
  }

  // Destructor
  template <typename GUM_SCALAR>
  BayesNetInference<GUM_SCALAR>::~BayesNetInference() {
    GUM_DESTRUCTOR(BayesNetInference);
    _invalidatePosteriors();
    for(auto p : __garbages)
      delete(p);
  }

  template <typename GUM_SCALAR>
  void BayesNetInference<GUM_SCALAR>::_invalidatePosteriors() {
    for (auto it : _posteriors) {
      if (it.second != nullptr)
        delete (it.second);
    }

    _posteriors.clear();
  }

  template <typename GUM_SCALAR>
  const Potential<GUM_SCALAR> &BayesNetInference<GUM_SCALAR>::posterior(NodeId id) {
    if (!_posteriors.exists(id)) {
      _posteriors.insert(id, new Potential<GUM_SCALAR>());
      _fillPosterior(id, *_posteriors[id]);
    }

    return *_posteriors[id];
  }

  template <typename GUM_SCALAR>
  const IBayesNet<GUM_SCALAR> &BayesNetInference<GUM_SCALAR>::bn() const {
    return __bayesNet;
  }

  template <typename GUM_SCALAR>
  void BayesNetInference<GUM_SCALAR>::addHardEvidence(NodeId id, Idx pos) {
    List<const Potential<GUM_SCALAR> *> pot_list;

    const DiscreteVariable &v = __bayesNet.variable(id);

    if (pos >= v.domainSize())
      GUM_ERROR(OutOfBounds, "Indice for variable " << id << " out of bounds.");

    Potential<GUM_SCALAR> *po = new Potential<GUM_SCALAR>();
    (*po) << v;
    po->fill(GUM_SCALAR(0));

    Instantiation I(*po);
    I.chgVal(v, pos);
    po->set(I, GUM_SCALAR(1));

    pot_list.insert(po);
    insertEvidence(pot_list);

    // remember to free it
    __garbages.insert(po);
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
// kate: indent-mode cstyle; indent-width 2; replace-tabs on;
