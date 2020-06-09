
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief Implementation of Loopy Belief Propagation in Bayesian Networks.
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <algorithm>
#  include <sstream>
#  include <string>

#  define LBP_DEFAULT_MAXITER          100
#  define LBP_DEFAULT_EPSILON          1e-8
#  define LBP_DEFAULT_MIN_EPSILON_RATE 1e-10
#  define LBP_DEFAULT_PERIOD_SIZE      1
#  define LBP_DEFAULT_VERBOSITY        false


// to ease parsing for IDE
#  include <agrum/BN/inference/loopyBeliefPropagation.h>


namespace gum {

  /// default constructor
  template < typename GUM_SCALAR >
  LoopyBeliefPropagation< GUM_SCALAR >::LoopyBeliefPropagation(
     const IBayesNet< GUM_SCALAR >* bn) :
      ApproximateInference< GUM_SCALAR >(bn) {
    // for debugging purposes
    GUM_CONSTRUCTOR(LoopyBeliefPropagation);

    this->setEpsilon(LBP_DEFAULT_EPSILON);
    this->setMinEpsilonRate(LBP_DEFAULT_MIN_EPSILON_RATE);
    this->setMaxIter(LBP_DEFAULT_MAXITER);
    this->setVerbosity(LBP_DEFAULT_VERBOSITY);
    this->setPeriodSize(LBP_DEFAULT_PERIOD_SIZE);

    init_messages__();
  }

  /// destructor
  template < typename GUM_SCALAR >
  INLINE LoopyBeliefPropagation< GUM_SCALAR >::~LoopyBeliefPropagation() {
    GUM_DESTRUCTOR(LoopyBeliefPropagation);
  }


  template < typename GUM_SCALAR >
  void LoopyBeliefPropagation< GUM_SCALAR >::init_messages__() {
    messages__.clear();
    for (const auto& tail: this->BN().nodes()) {
      Potential< GUM_SCALAR > p;
      p.add(this->BN().variable(tail));
      p.fill(static_cast< GUM_SCALAR >(1));

      for (const auto& head: this->BN().children(tail)) {
        messages__.insert(Arc(head, tail), p);
        messages__.insert(Arc(tail, head), p);
      }
    }
  }

  template < typename GUM_SCALAR >
  void LoopyBeliefPropagation< GUM_SCALAR >::updateOutdatedBNStructure_() {
    init_messages__();
  }


  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >
     LoopyBeliefPropagation< GUM_SCALAR >::computeProdPi__(NodeId X) {
    const auto& varX = this->BN().variable(X);

    auto piX = this->BN().cpt(X);
    for (const auto& U: this->BN().parents(X)) {
      piX *= messages__[Arc(U, X)];
    }
    piX = piX.margSumIn({&varX});

    return piX;
  }

  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >
     LoopyBeliefPropagation< GUM_SCALAR >::computeProdPi__(NodeId X,
                                                           NodeId except) {
    const auto& varX = this->BN().variable(X);
    const auto& varExcept = this->BN().variable(except);
    auto        piXexcept = this->BN().cpt(X);
    for (const auto& U: this->BN().parents(X)) {
      if (U != except) { piXexcept *= messages__[Arc(U, X)]; }
    }
    piXexcept = piXexcept.margSumIn({&varX, &varExcept});
    return piXexcept;
  }


  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >
     LoopyBeliefPropagation< GUM_SCALAR >::computeProdLambda__(NodeId X) {
    Potential< GUM_SCALAR > lamX;
    if (this->hasEvidence(X)) {
      lamX = *(this->evidence()[X]);
    } else {
      lamX.add(this->BN().variable(X));
      lamX.fill(1);
    }
    for (const auto& Y: this->BN().children(X)) {
      lamX *= messages__[Arc(Y, X)];
    }

    return lamX;
  }

  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >
     LoopyBeliefPropagation< GUM_SCALAR >::computeProdLambda__(NodeId X,
                                                               NodeId except) {
    Potential< GUM_SCALAR > lamXexcept;
    if (this->hasEvidence(X)) {   //
      lamXexcept = *this->evidence()[X];
    } else {
      lamXexcept.add(this->BN().variable(X));
      lamXexcept.fill(1);
    }
    for (const auto& Y: this->BN().children(X)) {
      if (Y != except) { lamXexcept *= messages__[Arc(Y, X)]; }
    }

    return lamXexcept;
  }


  template < typename GUM_SCALAR >
  GUM_SCALAR LoopyBeliefPropagation< GUM_SCALAR >::updateNodeMessage__(NodeId X) {
    auto piX = computeProdPi__(X);
    auto lamX = computeProdLambda__(X);

    GUM_SCALAR KL = 0;
    Arc        argKL(0, 0);

    // update lambda_par (for arc U->x)
    for (const auto& U: this->BN().parents(X)) {
      auto newLambda =
         (computeProdPi__(X, U) * lamX).margSumIn({&this->BN().variable(U)});
      newLambda.normalize();
      auto ekl = static_cast< GUM_SCALAR >(0);
      try {
        ekl = messages__[Arc(X, U)].KL(newLambda);
      } catch (InvalidArgument&) {
        GUM_ERROR(InvalidArgument, "Not compatible pi during computation");
      } catch (FatalError&) {   // 0 misplaced
        ekl = std::numeric_limits< GUM_SCALAR >::infinity();
      }
      if (ekl > KL) {
        KL = ekl;
        argKL = Arc(X, U);
      }
      messages__.set(Arc(X, U), newLambda);
    }

    // update pi_child (for arc x->child)
    for (const auto& Y: this->BN().children(X)) {
      auto newPi = (piX * computeProdLambda__(X, Y));
      newPi.normalize();
      GUM_SCALAR ekl = KL;
      try {
        ekl = messages__[Arc(X, Y)].KL(newPi);
      } catch (InvalidArgument&) {
        GUM_ERROR(InvalidArgument, "Not compatible pi during computation");
      } catch (FatalError&) {   // 0 misplaced
        ekl = std::numeric_limits< GUM_SCALAR >::infinity();
      }
      if (ekl > KL) {
        KL = ekl;
        argKL = Arc(X, Y);
      }
      messages__.set(Arc(X, Y), newPi);
    }

    return KL;
  }

  template < typename GUM_SCALAR >
  INLINE void LoopyBeliefPropagation< GUM_SCALAR >::initStats__() {
    init_messages__();
    for (const auto& node: this->BN().topologicalOrder()) {
      updateNodeMessage__(node);
    }
  }


  /// Returns the probability of the variables.
  template < typename GUM_SCALAR >
  void LoopyBeliefPropagation< GUM_SCALAR >::makeInference_() {
    initStats__();
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
        GUM_SCALAR e = updateNodeMessage__(node);
        if (e > error) error = e;
      }
    } while (this->continueApproximationScheme(error));
  }


  /// Returns the probability of the variable.
  template < typename GUM_SCALAR >
  INLINE const Potential< GUM_SCALAR >&
               LoopyBeliefPropagation< GUM_SCALAR >::posterior_(NodeId id) {
    auto p = computeProdPi__(id) * computeProdLambda__(id);
    p.normalize();
    posteriors__.set(id, p);

    return posteriors__[id];
  }
} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS
