/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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


/**
 * @file
 * @brief Implementation of Loopy Belief Propagation in Bayesian networks.
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <algorithm>
#  include <sstream>
#  include <string>

constexpr auto LBP_DEFAULT_MAXITER          = 100;
constexpr auto LBP_DEFAULT_EPSILON          = 1e-8;
constexpr auto LBP_DEFAULT_MIN_EPSILON_RATE = 1e-10;
constexpr auto LBP_DEFAULT_PERIOD_SIZE      = 1;
constexpr auto LBP_DEFAULT_VERBOSITY        = false;


// to ease parsing for IDE
#  include <agrum/BN/inference/loopyBeliefPropagation.h>


namespace gum {

  /// default constructor
  template < typename GUM_SCALAR >
  LoopyBeliefPropagation< GUM_SCALAR >::LoopyBeliefPropagation(const IBayesNet< GUM_SCALAR >* bn) :
      ApproximateInference< GUM_SCALAR >(bn) {
    // for debugging purposes
    GUM_CONSTRUCTOR(LoopyBeliefPropagation)

    this->setEpsilon(LBP_DEFAULT_EPSILON);
    this->setMinEpsilonRate(LBP_DEFAULT_MIN_EPSILON_RATE);
    this->setMaxIter(LBP_DEFAULT_MAXITER);
    this->setVerbosity(LBP_DEFAULT_VERBOSITY);
    this->setPeriodSize(LBP_DEFAULT_PERIOD_SIZE);

    _init_messages_();
  }

  /// destructor
  template < typename GUM_SCALAR >
  INLINE LoopyBeliefPropagation< GUM_SCALAR >::~LoopyBeliefPropagation() {
    GUM_DESTRUCTOR(LoopyBeliefPropagation)
  }


  template < typename GUM_SCALAR >
  void LoopyBeliefPropagation< GUM_SCALAR >::_init_messages_() {
    _messages_.clear();
    for (const auto& tail: this->BN().nodes()) {
      Potential< GUM_SCALAR > p;
      p.add(this->BN().variable(tail));
      p.fill(static_cast< GUM_SCALAR >(1));

      for (const auto& head: this->BN().children(tail)) {
        _messages_.insert(Arc(head, tail), p);
        _messages_.insert(Arc(tail, head), p);
      }
    }
  }

  template < typename GUM_SCALAR >
  void LoopyBeliefPropagation< GUM_SCALAR >::updateOutdatedStructure_() {
    _init_messages_();
  }


  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR > LoopyBeliefPropagation< GUM_SCALAR >::_computeProdPi_(NodeId X) {
    const auto& varX = this->BN().variable(X);

    auto piX = this->BN().cpt(X);
    for (const auto& U: this->BN().parents(X)) {
      piX *= _messages_[Arc(U, X)];
    }
    piX = piX.margSumIn({&varX});

    return piX;
  }

  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR > LoopyBeliefPropagation< GUM_SCALAR >::_computeProdPi_(NodeId X,
                                                                                NodeId except) {
    const auto& varX      = this->BN().variable(X);
    const auto& varExcept = this->BN().variable(except);
    auto        piXexcept = this->BN().cpt(X);
    for (const auto& U: this->BN().parents(X)) {
      if (U != except) { piXexcept *= _messages_[Arc(U, X)]; }
    }
    piXexcept = piXexcept.margSumIn({&varX, &varExcept});
    return piXexcept;
  }


  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR > LoopyBeliefPropagation< GUM_SCALAR >::_computeProdLambda_(NodeId X) {
    Potential< GUM_SCALAR > lamX;
    if (this->hasEvidence(X)) {
      lamX = *(this->evidence()[X]);
    } else {
      lamX.add(this->BN().variable(X));
      lamX.fill(1);
    }
    for (const auto& Y: this->BN().children(X)) {
      lamX *= _messages_[Arc(Y, X)];
    }

    return lamX;
  }

  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR > LoopyBeliefPropagation< GUM_SCALAR >::_computeProdLambda_(NodeId X,
                                                                                    NodeId except) {
    Potential< GUM_SCALAR > lamXexcept;
    if (this->hasEvidence(X)) {   //
      lamXexcept = *this->evidence()[X];
    } else {
      lamXexcept.add(this->BN().variable(X));
      lamXexcept.fill(1);
    }
    for (const auto& Y: this->BN().children(X)) {
      if (Y != except) { lamXexcept *= _messages_[Arc(Y, X)]; }
    }

    return lamXexcept;
  }


  template < typename GUM_SCALAR >
  GUM_SCALAR LoopyBeliefPropagation< GUM_SCALAR >::_updateNodeMessage_(NodeId X) {
    auto piX  = _computeProdPi_(X);
    auto lamX = _computeProdLambda_(X);

    GUM_SCALAR KL = 0;
    Arc        argKL(0, 0);

    // update lambda_par (for arc U->x)
    for (const auto& U: this->BN().parents(X)) {
      auto newLambda = (_computeProdPi_(X, U) * lamX).margSumIn({&this->BN().variable(U)});
      newLambda.normalize();
      auto ekl = static_cast< GUM_SCALAR >(0);
      try {
        ekl = _messages_[Arc(X, U)].KL(newLambda);
      } catch (InvalidArgument const&) {
        GUM_ERROR(InvalidArgument, "Not compatible pi during computation")
      } catch (FatalError const&) {   // 0 misplaced
        ekl = std::numeric_limits< GUM_SCALAR >::infinity();
      }
      if (ekl > KL) {
        KL    = ekl;
        argKL = Arc(X, U);
      }
      _messages_.set(Arc(X, U), newLambda);
    }

    // update pi_child (for arc x->child)
    for (const auto& Y: this->BN().children(X)) {
      auto newPi = (piX * _computeProdLambda_(X, Y));
      newPi.normalize();
      GUM_SCALAR ekl = KL;
      try {
        ekl = _messages_[Arc(X, Y)].KL(newPi);
      } catch (InvalidArgument const&) {
        GUM_ERROR(InvalidArgument, "Not compatible pi during computation")
      } catch (FatalError const&) {   // 0 misplaced
        ekl = std::numeric_limits< GUM_SCALAR >::infinity();
      }
      if (ekl > KL) {
        KL    = ekl;
        argKL = Arc(X, Y);
      }
      _messages_.set(Arc(X, Y), newPi);
    }

    return KL;
  }

  template < typename GUM_SCALAR >
  INLINE void LoopyBeliefPropagation< GUM_SCALAR >::_initStats_() {
    _init_messages_();
    for (const auto& node: this->BN().topologicalOrder()) {
      _updateNodeMessage_(node);
    }
  }


  /// Returns the probability of the variables.
  template < typename GUM_SCALAR >
  void LoopyBeliefPropagation< GUM_SCALAR >::makeInference_() {
    _initStats_();
    this->initApproximationScheme();

    std::vector< NodeId > shuffleIds;
    for (const auto& node: this->BN().nodes())
      shuffleIds.push_back(node);

    auto engine = std::default_random_engine{};

    GUM_SCALAR error = 0.0;
    do {
      std::shuffle(std::begin(shuffleIds), std::end(shuffleIds), engine);
      this->updateApproximationScheme();
      for (const auto& node: shuffleIds) {
        GUM_SCALAR e = _updateNodeMessage_(node);
        if (e > error) error = e;
      }
    } while (this->continueApproximationScheme(error));
  }


  /// Returns the probability of the variable.
  template < typename GUM_SCALAR >
  INLINE const Potential< GUM_SCALAR >&
               LoopyBeliefPropagation< GUM_SCALAR >::posterior_(NodeId id) {
    auto p = _computeProdPi_(id) * _computeProdLambda_(id);
    p.normalize();
    _posteriors_.set(id, p);

    return _posteriors_[id];
  }
} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS
