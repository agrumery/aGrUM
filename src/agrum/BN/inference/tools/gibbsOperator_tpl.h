
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
 * @brief Implementation of Gibbs inference methods in Bayesian Networks.
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN
 */


#include <agrum/BN/inference/tools/gibbsOperator.h>
#include <agrum/core/utils_random.h>

namespace gum {

  template < typename GUM_SCALAR >
  GibbsOperator< GUM_SCALAR >::GibbsOperator(const IBayesNet< GUM_SCALAR >& BN,
                                             const NodeProperty< Idx >*     hardEv,
                                             Size                           nbr,
                                             bool atRandom) :
      _counting(0),
      _sampling_bn(BN), _hardEv(hardEv), _nbr(nbr), _atRandom(atRandom) {
    __updateSamplingNodes();
    GUM_CONSTRUCTOR(GibbsOperator);
  }

  template < typename GUM_SCALAR >
  GibbsOperator< GUM_SCALAR >::~GibbsOperator() {
    GUM_DESTRUCTOR(GibbsOperator);
  }

  template < typename GUM_SCALAR >
  void GibbsOperator< GUM_SCALAR >::__updateSamplingNodes() {
    _samplingNodes.clear();
    for (const auto node : _sampling_bn.nodes())
      if (_hardEv == nullptr || !_hardEv->exists(node))
        _samplingNodes.insert(node);
    if (_samplingNodes.size() == 0) {
      GUM_ERROR(InvalidArgument,
                "No node to sample (too many nodes or too much evidence)!")
    }
    if (_nbr > _samplingNodes.size()) _nbr = _samplingNodes.size();
  }

  /// returns a MC sample
  /// This is not a really sample since we take into account evidence without
  /// care about parent of evidence, etc.
  /// This is just a not-so-bad first sample for GibbsSampler
  template < typename GUM_SCALAR >
  Instantiation GibbsOperator< GUM_SCALAR >::monteCarloSample() {
    gum::Instantiation I;

    for (const auto nod : _sampling_bn.topologicalOrder()) {
      I.add(_sampling_bn.variable(nod));
      if (_hardEv != nullptr && _hardEv->exists(nod)) {
        I.chgVal(_sampling_bn.variable(nod), (*_hardEv)[nod]);
      } else {
        __drawVarMonteCarlo(nod, &I);
      }
    }
    return I;
  }

  template < typename GUM_SCALAR >
  void GibbsOperator< GUM_SCALAR >::__drawVarMonteCarlo(NodeId         nod,
                                                        Instantiation* I) {
    gum::Instantiation Itop(*I);
    Itop.erase(_sampling_bn.variable(nod));
    I->chgVal(_sampling_bn.variable(nod),
              _sampling_bn.cpt(nod).extract(Itop).draw());
  }


  template < typename GUM_SCALAR >
  Instantiation GibbsOperator< GUM_SCALAR >::nextSample(Instantiation prev) {
    for (Idx i = 0; i < _nbr; i++) {
      auto pos = _atRandom ? randomValue(_samplingNodes.size())
                           : (_counting % _samplingNodes.size());
      this->__GibbsSample(_samplingNodes[pos], &prev);
      _counting++;
    }
    return prev;
  }
  /// change in Instantiation I a new drawn value for id

  template < typename GUM_SCALAR >
  void GibbsOperator< GUM_SCALAR >::__GibbsSample(NodeId id, Instantiation* I) {
    gum::Instantiation Itop(*I);
    Itop.erase(_sampling_bn.variable(id));
    gum::Potential< GUM_SCALAR > p = _sampling_bn.cpt(id).extract(Itop);
    for (const auto nod : _sampling_bn.children(id))
      p *= _sampling_bn.cpt(nod).extract(Itop);
    GUM_ASSERT(p.nbrDim() == 1);
    if (p.sum() != 0) {
      p.normalize();
      I->chgVal(_sampling_bn.variable(id), p.draw());
    }
  }
}   // namespace gum
