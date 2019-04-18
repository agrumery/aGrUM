
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Implementation of Estimator for approximate inference in bayesian
 * networks
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN
 */

namespace gum {

  template < typename GUM_SCALAR >
  Estimator< GUM_SCALAR >::Estimator() {
    GUM_CONSTRUCTOR(Estimator);
    _wtotal = (GUM_SCALAR)0.;
    _ntotal = (Size)0;
    _bn = nullptr;
  }


  template < typename GUM_SCALAR >
  Estimator< GUM_SCALAR >::Estimator(const IBayesNet< GUM_SCALAR >* bn) :
      Estimator() {
    _bn = bn;

    for (gum::NodeGraphPartIterator iter = bn->nodes().begin();
         iter != bn->nodes().end();
         ++iter)
      _estimator.insert(
         bn->variable(*iter).name(),
         std::vector< GUM_SCALAR >(bn->variable(*iter).domainSize(), 0.0));

    GUM_CONSTRUCTOR(Estimator);
  }


  template < typename GUM_SCALAR >
  INLINE Estimator< GUM_SCALAR >::~Estimator() {
    GUM_DESTRUCTOR(Estimator);
    // remove all the posteriors computed
    clear();
  }


  /* adds all potential target variables from a given BN to the Estimator */

  template < typename GUM_SCALAR >
  void Estimator< GUM_SCALAR >::setFromBN(const IBayesNet< GUM_SCALAR >* bn,
                                          const NodeSet& hardEvidence) {
    for (gum::NodeGraphPartIterator iter = bn->nodes().begin();
         iter != bn->nodes().end();
         ++iter) {
      auto v = bn->variable(*iter).name();

      if (!hardEvidence.contains(*iter)) {
        if (_estimator.exists(v))
          _estimator[v] = std::vector< GUM_SCALAR >(
             bn->variable(*iter).domainSize(), (GUM_SCALAR)0.0);
        else
          _estimator.insert(v,
                            std::vector< GUM_SCALAR >(
                               bn->variable(*iter).domainSize(), (GUM_SCALAR)0.0));
      }
    }
  }

  // we multiply the posteriors obtained by LoopyBeliefPropagation by the it's
  // number of iterations
  template < typename GUM_SCALAR >
  void
     Estimator< GUM_SCALAR >::setFromLBP(LoopyBeliefPropagation< GUM_SCALAR >* lbp,
                                         const NodeSet& hardEvidence,
                                         GUM_SCALAR     virtualLBPSize) {
    for (const auto& node : lbp->BN().nodes()) {
      if (!hardEvidence.contains(node)) {
        std::vector< GUM_SCALAR > v;
        auto                      p = lbp->posterior(node);
        gum::Instantiation        inst(p);

        for (inst.setFirst(); !inst.end(); ++inst) {
          v.push_back(p[inst] * virtualLBPSize);
        }

        _estimator.insert(lbp->BN().variable(node).name(), v);
      }
    }
    _ntotal = (Size)virtualLBPSize;
    _wtotal = virtualLBPSize;
  }

  /*update the Estimator given an instantiation I with weight bias w*/

  template < typename GUM_SCALAR >
  void Estimator< GUM_SCALAR >::update(Instantiation I, GUM_SCALAR w) {
    _wtotal += w;
    _ntotal += (Size)1;

    for (Idx i = 0; i < I.nbrDim(); i++) {
      if (_estimator.exists(I.variable(i).name()))
        _estimator[I.variable(i).name()][I.val(i)] += w;
    }
  }

  /* returns the approximation CPT of a variable */

  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >&
     Estimator< GUM_SCALAR >::posterior(const DiscreteVariable& var) {
    Potential< GUM_SCALAR >* p = nullptr;

    if (!_estimator.exists(var.name()))
      GUM_ERROR(NotFound, "Target variable not found");

    // check if we have already computed the posterior
    if (__target_posteriors.exists(var.name())) {
      p = __target_posteriors[var.name()];
    } else {
      p = new Potential< GUM_SCALAR >();
      *p << var;
      __target_posteriors.insert(var.name(), p);
    }

    p->fillWith(_estimator[var.name()]);
    p->normalize();
    return *p;
  }


  /* expected value considering a Bernouilli variable with parameter val */

  template < typename GUM_SCALAR >
  GUM_SCALAR Estimator< GUM_SCALAR >::EV(std::string name, Idx val) {
    return _estimator[name][val] / _wtotal;
  }


  /* variance considering a Bernouilli variable with parameter val */

  template < typename GUM_SCALAR >
  GUM_SCALAR Estimator< GUM_SCALAR >::variance(std::string name, Idx val) {
    GUM_SCALAR p = EV(name, val);
    return p * (1 - p);
  }


  /* returns maximum length of confidence intervals for each variable, each
   * parameter */

  template < typename GUM_SCALAR >
  GUM_SCALAR Estimator< GUM_SCALAR >::confidence() {
    GUM_SCALAR ic_max = 0;

    for (auto iter = _estimator.begin(); iter != _estimator.end(); ++iter) {
      for (Idx i = 0; i < iter.val().size(); i++) {
        GUM_SCALAR ic = GUM_SCALAR(
           2 * 1.96 * std::sqrt(variance(iter.key(), i) / (_ntotal - 1)));
        if (ic > ic_max) ic_max = ic;
      }
    }

    return ic_max;
  }

  template < typename GUM_SCALAR >
  void Estimator< GUM_SCALAR >::clear() {
    _estimator.clear();
    _wtotal = (GUM_SCALAR)0;
    _ntotal = Size(0);
    for (const auto& pot : __target_posteriors)
      delete pot.second;
    __target_posteriors.clear();
  }
}   // namespace gum
