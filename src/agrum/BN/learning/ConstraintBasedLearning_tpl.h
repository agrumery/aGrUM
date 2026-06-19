#pragma once

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/learning/paramUtils/DAG2BNLearner.h>

namespace gum {

  namespace learning {

    template < GUM_Numeric GUM_SCALAR, typename GRAPH_CHANGES_SELECTOR, typename PARAM_ESTIMATOR >
    BayesNet< GUM_SCALAR > ConstraintBasedLearning::learnBN(GRAPH_CHANGES_SELECTOR& selector,
                                                             PARAM_ESTIMATOR&        estimator,
                                                             DAG                     initial_dag) {
      return DAG2BNLearner::createBN< GUM_SCALAR >(estimator, learnStructure(selector, initial_dag));
    }

  } /* namespace learning */

} /* namespace gum */
