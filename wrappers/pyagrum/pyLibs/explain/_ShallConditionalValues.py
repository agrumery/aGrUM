# Imports
from pyagrum.explain._ShallValues import ShallValues
from pyagrum.explain._CustomShapleyCache import CustomShapleyCache
from pyagrum.explain._FIFOCache import FIFOCache
from pyagrum.explain._ComputationConditional import ConditionalComputation
# Calculations
import numpy as np
import pandas as pd

import pyagrum as gum

import warnings
    
class ConditionalShallValues(ShallValues, ConditionalComputation) :
    """
    The ConditionalShapValues class computes the conditional Shapley values for a given target node in a Bayesian Network.
    """

    def __init__(self, bn:gum.BayesNet, background:tuple|None, sample_size:int=1000, log:bool=True) :
        """

        Rows that contain nan values will be dropped.
        params:
        ------
        :bn -> The Bayesian Network.
        :target -> The node id of the target.
        :logit -> If True, applies the logit transformation to the probabilities.

        raises:
        ------
        :TypeError -> If bn is not a gum.BayesNet instance.
        :ValueError -> If target is not a valid node id in the Bayesian Network.
        :UserWarning -> If logit is not a boolean, a warning is issued.
        """

        super().__init__(bn, background, sample_size, log) # Initializes the ShapleyValues class.
        #self.baseline = self._value(self._data, self.vars_ids, {})
        self.baseline = self._value(data=self._data,
                                    counts=self.counts,
                                    elements=self.vars_ids,
                                    sigma=[],
                                    cache=FIFOCache(100),
                                    func1=self._joint,
                                    params1={},
                                    func2=self._weight,
                                    params2={}) 
    
    def _coalition_contribution(self, k, ex, feature, nodes_id, nodes_vals, cache, fifo_cache) :
        # key2 is always set since subsets are sorted by length
        key1, key2, _ = cache.generate_keys(self.bn, None, feature, nodes_id)
        # key1 : nodes_id, key2 : nodes id without feature
        if k == 0 :
            idx = self._extract(self._data, nodes_id, nodes_vals)
            cache.set(ex, key1, self._value(data=self._data[idx],
                                                    counts=self.counts[idx],
                                                    elements=self.vars_ids,
                                                    sigma=[],
                                                    cache=fifo_cache,
                                                    func1=self._joint,
                                                    params1={},
                                                    func2=self._weight,
                                                    params2={}))

        joint_prob_with = cache.get(ex, key1) # with feature
        joint_prob_without = cache.get(ex, key2) if len( key1 ) > 1 else cache.get(-1, '') # without feature
        return (joint_prob_with - joint_prob_without) / self._invcoeff_shap(len(self.vars_ids), len(nodes_id) - 1)

    def _shall_1dim(self, x)-> np.ndarray :
        contributions = np.zeros((self.M)) # Initializes contributions array.
        fifo_cache = FIFOCache(2000)
        cache = CustomShapleyCache(5000) # Initializes the custom cache.
        cache.set(-1, '', self.baseline) # Sets the baseline probability in the cache.
        coalitions = self._coalitions(self.vars_ids) # Generates coalitions.
        for nodes_id in coalitions :
            nodes_vals = x[nodes_id] # Gets the values of the nodes in the coalition.
            for k, feature in enumerate(nodes_id) :
                # Accumulates the contribution for each feature.
                contributions[feature] += self._coalition_contribution(k,
                                                                       0,
                                                                       int(feature),
                                                                       nodes_id,
                                                                       nodes_vals,
                                                                       cache,
                                                                       fifo_cache)
        return contributions

    def _shall_ndim(self, x) -> np.ndarray :
        contributions = np.zeros((self.M, len(x))) # Initializes contributions array.
        fifo_cache = FIFOCache(2000)
        cache = CustomShapleyCache(5000) # Initializes the custom cache.
        cache.set(-1, '', self.baseline) # Sets the baseline probability in the cache.

        coalitions = self._coalitions(self.vars_ids) # Generates coalitions.
        for i, nodes_id in enumerate(coalitions):
            data_vals = x[:, nodes_id] # Gets the values of the nodes in the coalition.
            for ex, nodes_vals in enumerate(data_vals) :
                for k, feature in enumerate( nodes_id ) :
                    contributions[feature, ex] += self._coalition_contribution(k,
                                                                               ex,
                                                                               int(feature),
                                                                               nodes_id,
                                                                               nodes_vals,
                                                                               cache,
                                                                               fifo_cache)
        return contributions