# Imports

import pyagrum as gum
from pyagrum.explain._ShallValues import ShallValues
from pyagrum.explain._ComputationMarginal import MarginalComputation
from pyagrum.explain._CustomShapleyCache import CustomShapleyCache
from pyagrum.explain._FIFOCache import FIFOCache


# Calculations
import numpy as np
# aGrUM
# GL
import warnings

# Imports

# Calculations
import pandas as pd
import numpy as np

# aGrUM
import pyagrum as gum
# GL
import warnings

# Au lieu de logit, faire log pour faire l'entropie
class MarginalShallValues(ShallValues, MarginalComputation):
    """
    The MarginalShapValues class computes the Marginal Shapley values for a given target node in a Bayesian Network.
    """

    def __init__(self, bn: gum.BayesNet, background: tuple | None, sample_size:int=1000, log:bool=True):
        """
        params:
        ------
        :bn -> The Bayesian Network.
        :background -> If None then sample of size 'sample_size' would be generated from the Bayesian Network. Otherwise, should be 
            a tuple of a dataframe and a boolean = True if there are strings in a dataframe and False otherwise.
        :logit -> If True, applies the logit transformation to the probabilities.

        Raises:
        ------
        :TypeError -> If bn is not a gum.BayesNet instance.
        :ValueError -> If target is not a valid node id in the Bayesian Network.
        :UserWarning -> If logit is not a boolean, a warning is issued.s
        """
        super().__init__(bn, background, sample_size, log)       
        
        self.baseline = self._value(data=self._data,
                                    counts=self.counts,
                                    elements=self.vars_ids,
                                    sigma=[],
                                    cache=FIFOCache(100),
                                    func1=self._joint,
                                    params1={},
                                    func2=self._weight,
                                    params2={}) 

    def _coalition_contribution(self,
                                 k, 
                                 ex,
                                 feature,
                                 fifo_cache,
                                 nodes_id, 
                                 nodes_vals, 
                                 cache) :
        
        key1, key2, _ = cache.generate_keys(self.bn, None, feature, nodes_id)
        if k == 0 :
            interv = self._data.copy()
            interv[:, nodes_id] = nodes_vals
            cache.set(ex, key1, self._value(data=interv,
                                            counts=self.counts,
                                            elements=self.vars_ids,
                                            sigma=[],
                                            cache=fifo_cache,
                                            func1=self._joint,
                                            params1={},
                                            func2=self._weight,
                                            params2={} ))

        joint_prob_with = cache.get(ex, key1)
        joint_prob_without = cache.get(ex, key2) if len( key1 ) > 1 else cache.get(-1, ())
        return (joint_prob_with - joint_prob_without) / self._invcoeff_shap(len(self.vars_ids), len(nodes_id) - 1)

    def _shall_1dim(self, x):
        # Result initialisation.
        contributions = np.zeros(self.M)
        # Cache management.
        fifo_cache = FIFOCache(2000)
        custom_cache = CustomShapleyCache(5000)
        # Sets the baseline probability in the cache.
        custom_cache.set(-1, (), self.baseline)
        coalitions = self._coalitions(self.vars_ids)
        for nodes_id in coalitions:
            nodes_vals = x[nodes_id]
            for k, feature in enumerate(nodes_id):
                contributions[feature] += self._coalition_contribution(k,
                                                                       0,
                                                                       int(feature),
                                                                       fifo_cache,
                                                                       nodes_id,
                                                                       nodes_vals,
                                                                       custom_cache)
        return contributions

    def _shall_ndim(self, x):
        # Result initialisation.
        contributions = np.zeros((self.M, len(x)))
        # Cache management.
        fifo_cache = FIFOCache(2000)
        custom_cache = CustomShapleyCache(5000)
        # Sets the baseline probability in the cache.
        custom_cache.set(-1, (), self.baseline)
        coalitions = self._coalitions(self.vars_ids)
        for nodes_id in coalitions:
            for ex, nodes_values in enumerate(x[:, nodes_id]):
                for k, feature in enumerate(nodes_id):
                    contributions[feature, ex] += self._coalition_contribution(k,
                                                                               ex,
                                                                               int(feature),
                                                                               fifo_cache,
                                                                               nodes_id,
                                                                               nodes_values,
                                                                               custom_cache)
        return contributions
