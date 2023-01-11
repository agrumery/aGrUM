/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
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

#include <agrum/BN/learning/priors/DirichletPriorFromBN.h>

/** @file
 * @brief A dirichlet priori: computes its N'_ijk from a database
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum::learning {

  /// default constructor
  template < typename GUM_SCALAR >
  DirichletPriorFromBN< GUM_SCALAR >::DirichletPriorFromBN(const DatabaseTable& learning_db,
                                                           const BayesNet< GUM_SCALAR >* priorbn) :
      Prior(learning_db),
      _prior_bn_(priorbn) {
    GUM_CONSTRUCTOR(DirichletPriorFromBN)
  }


  /// copy constructor
  template < typename GUM_SCALAR >
  DirichletPriorFromBN< GUM_SCALAR >::DirichletPriorFromBN(const DirichletPriorFromBN& from) :
      Prior(from), _prior_bn_(from._prior_bn_) {
    GUM_CONS_CPY(DirichletPriorFromBN)
  }


  /// move constructor
  template < typename GUM_SCALAR >
  DirichletPriorFromBN< GUM_SCALAR >::DirichletPriorFromBN(DirichletPriorFromBN&& from) noexcept :
      Prior(std::move(from)), _prior_bn_(std::move(from._prior_bn_)) {
    GUM_CONS_MOV(DirichletPriorFromBN)
  }


  /// virtual copy constructor
  template < typename GUM_SCALAR >
  DirichletPriorFromBN< GUM_SCALAR >* DirichletPriorFromBN< GUM_SCALAR >::clone() const {
    return new DirichletPriorFromBN(*this);
  }


  /// destructor

  template < typename GUM_SCALAR >
  DirichletPriorFromBN< GUM_SCALAR >::~DirichletPriorFromBN() {
    GUM_DESTRUCTOR(DirichletPriorFromBN)
  }


  /// copy operator
  template < typename GUM_SCALAR >
  DirichletPriorFromBN< GUM_SCALAR >&
     DirichletPriorFromBN< GUM_SCALAR >::operator=(const DirichletPriorFromBN& from) {
    if (this != &from) {
      Prior::operator=(from);
      _prior_bn_ = from._prior_bn_;
    }
    return *this;
  }


  /// move operator
  template < typename GUM_SCALAR >
  DirichletPriorFromBN< GUM_SCALAR >&
     DirichletPriorFromBN< GUM_SCALAR >::operator=(DirichletPriorFromBN&& from) {
    if (this != &from) {
      Prior::operator=(std::move(from));
      _prior_bn_ = from._prior_bn_;
    }
    return *this;
  }


  /// returns the type of the prior

  template < typename GUM_SCALAR >
  INLINE PriorType DirichletPriorFromBN< GUM_SCALAR >::getType() const {
    return PriorType::DirichletPriorType;
  }


  /// indicates whether the prior is potentially informative

  template < typename GUM_SCALAR >
  INLINE bool DirichletPriorFromBN< GUM_SCALAR >::isInformative() const {
    return (this->weight_ != 0.0);
  }


  /// sets the weight of the a prior(kind of effective sample size)
  template < typename GUM_SCALAR >
  INLINE void DirichletPriorFromBN< GUM_SCALAR >::setWeight(const double weight) {
    Prior::setWeight(weight);
  }


  /// returns the prior vector all the variables in the idset
  template < typename GUM_SCALAR >
  INLINE void
     DirichletPriorFromBN< GUM_SCALAR >::addJointPseudoCount(const IdCondSet&       idset,
                                                             std::vector< double >& counts) {
    if (this->weight_ == 0.0) return;
    const auto [X, Y] = idset.toNodeSets();

    gum::Instantiation Ijoint;
    for (auto i = std::size_t(0); i < idset.size(); i++) {
      Ijoint.add(_prior_bn_->variable(idset.ids()[i]));
    }

    _addCountsForJoint_(Ijoint, X + Y, counts);
  }


  /// returns the prior vector over only the conditioning set of an idset
  template < typename GUM_SCALAR >
  INLINE void
     DirichletPriorFromBN< GUM_SCALAR >::addConditioningPseudoCount(const IdCondSet&       idset,
                                                                    std::vector< double >& counts) {
    if (this->weight_ == 0.0) return;
    const auto [X, Y] = idset.toNodeSets();
    gum::Instantiation Ijoint;
    for (auto i = idset.nbLHSIds(); i < idset.size(); i++)
      Ijoint.add(_prior_bn_->variable(idset.ids()[i]));
    _addCountsForJoint_(Ijoint, Y, counts);
  }

  template < typename GUM_SCALAR >
  void DirichletPriorFromBN< GUM_SCALAR >::_addCountsForJoint_(Instantiation&         Ijoint,
                                                               const NodeSet&         joint,
                                                               std::vector< double >& counts) {
    const auto size = counts.size();
    if (size != Ijoint.domainSize())
      GUM_ERROR(ArgumentError,
                "The size of counts ("
                   << size << ") does not match with the size of the asked prior (" << joint
                   << " : " << Ijoint.domainSize() << ")")

    if (joint.empty()) {
      counts[0] += weight_;
      return;
    }

    LazyPropagation lazy(_prior_bn_);

    Potential< GUM_SCALAR > p;
    if (joint.size() == 1) {
      const auto& target = *joint.begin();
      lazy.addTarget(target);
      lazy.makeInference();
      p = lazy.posterior(target);
    } else {
      lazy.addJointTarget(joint);
      lazy.makeInference();
      p = lazy.jointPosterior(joint);
    }
    if (weight_ != 1) p.scale(weight_);

    Ijoint.setFirst();
    for (auto i = std::size_t(0); i < size; ++i) {
      counts[i] += p[Ijoint];
      Ijoint.inc();
    }
  }
}   // namespace gum::learning

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
