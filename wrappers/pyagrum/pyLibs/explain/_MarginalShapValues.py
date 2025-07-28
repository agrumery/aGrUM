# Imports
from pyagrum.explain._ShapleyValues import ShapleyValues
from pyagrum.explain._CustomShapleyCache import CustomShapleyCache
from pyagrum.explain._FIFOCache import FIFOCache
# Calculations
import pandas as pd
import numpy as np
# aGrUM
import pyagrum as gum
# GL
import warnings

class MarginalShapValues(ShapleyValues) :
    """
    The MarginalShapValues class computes the Marginal Shapley values for a given target node in a Bayesian Network.
    """

    def __init__(self, bn: gum.BayesNet, target: int, background: tuple | None, sample_size=1000, logit=True) :
        """
        params:
        ------
        :bn -> The Bayesian Network.
        :target -> The node id of the target.
        :logit -> If True, applies the logit transformation to the probabilities.

        Raises:
        ------
        :TypeError -> If bn is not a gum.BayesNet instance.
        :ValueError -> If target is not a valid node id in the Bayesian Network.
        :UserWarning -> If logit is not a boolean, a warning is issued.
        """
        super().__init__(bn, target, logit)

        # Meta informations ...
        self._mb = self._markov_blanket()

        # Processing background data 
        if background is None :
            if not isinstance(sample_size, int) :
                raise TypeError("When `data`=None, `sample_size` must be an integer, but got {}".format(type(sample_size)))
            else :
                if sample_size < 1 :
                    raise ValueError("`sample_size` must be greater than 1, but got {}".format(sample_size))
                elif sample_size < 10 :
                    warnings.warn("The sample size is small, which may lead to biased Shapley values.")
            data = gum.generateSample(self.bn, sample_size, with_labels=False)[0].reindex(columns=self.feat_names).to_numpy()
        else :
            if not isinstance(background, tuple) :
                raise TypeError(f'`background` must be a tuple (pd.DataFrame, bool).')
            data, with_labels = background
            if not isinstance(with_labels, bool) :
                warnings.warn(f'The second element of `background` should be a boolean, but got {type(with_labels)}. Unexpected calculations may occur.')
            if not isinstance(data, pd.DataFrame) :
                raise TypeError("The first element of `background` must be a pandas DataFrame, but got {}".format(type(data)))
            if data.shape[0] < 2 :
                warnings.warn("You are giving a single row as a background data, which will lead to biased Shapley values.")
            if data.shape[1] != self.M :
                raise ValueError("The number of columns in the background data must match the number of variables in the Bayesian network. Although values outside the Markov blanket, including the target, are unused, they are required for indexing purposes.")
            data = data.reindex(columns=self.feat_names).to_numpy()
            if with_labels :
                data = self._labelToPos_df(data, [i for i in range(self.M) if i != self.target])

        self._data, self.counts = np.unique(data, axis=0, return_counts=True)
        self._N = int(np.sum(self.counts))
        if self._N == 0 :
            raise ValueError("Background data can't be empty.")
        self.baseline = self._value(self._data,
                                    [i for i in range(self._data.shape[1]) if i != self.target],
                                    {})
    
    def _markov_blanket(self) :
        mb = gum.MarkovBlanket(self.bn, self.target).nodes()
        mb.remove(self.target)
        return sorted(list(mb))

    def _value(self, interv: np.array, elements: list[int], markovImpact: FIFOCache) :
        val = np.zeros( self.bn.variable(self.target).domainSize(), dtype=float )
        for i in range(len(interv)) :
            posterior = markovImpact.get(tuple(interv[i, elements]), None)
            if posterior is None :
               evidces = {key: int(interv[i, key]) for key in elements}
               self.ie.updateEvidence(evidces)
               posterior = self.ie.posterior(self.target).toarray()
               markovImpact[tuple(interv[i, elements])] = posterior
            val += posterior * self.counts[i]

        self.ie.eraseAllEvidence()
        return self.func( val / self._N )
    
    def _coalition_contribution(self,
                                 k, 
                                 ex,
                                 elements, 
                                 feature,
                                 markovImpact,
                                 nodes_id, 
                                 nodes_vals, 
                                 cache) :
        
        key1, key2, _ = cache.generate_keys(self.bn, self.target, feature, nodes_id)
        if k == 0 :
            interv = self._data.copy()
            interv[:, nodes_id] = nodes_vals
            cache.set(ex, key1, self._value(interv, elements, markovImpact))

        posterior_prob_with = cache.get(ex, key1)
        posterior_prob_without = cache.get(ex, key2) if len( key1 ) > 1 else cache.get(-1, ())
        return (posterior_prob_with - posterior_prob_without) / self._invcoeff_shap(len(elements), len(nodes_id) - 1)
    
    def _shap_1dim(self, x, elements):
        # Result initialisation.
        contributions = np.zeros( (self.M, self.bn.variable(self.target).domainSize()) )
        # Cache management.
        markovImpact = FIFOCache(2000)
        cache = CustomShapleyCache(5000)
        # Sets the baseline probability in the cache.
        cache.set( -1, (), self.baseline)
        coalitions = self._coalitions(elements)
        for nodes_id in coalitions :
            nodes_vals = x[nodes_id]
            for k, feature in enumerate(nodes_id) :
                contributions[feature] += self._coalition_contribution(k,
                                                                       0,
                                                                       elements,
                                                                       int(feature),
                                                                       markovImpact,
                                                                       nodes_id,
                                                                       nodes_vals,
                                                                       cache)
        return contributions

    def _shap_ndim(self, x, elements) :
        # Result initialisation.
        contributions = np.zeros((self.M, len(x), self.bn.variable(self.target).domainSize()))
        # Cache management.
        markovImpact = FIFOCache(2000)
        cache = CustomShapleyCache(5000)
        # Sets the baseline probability in the cache.
        cache.set( -1, (), self.baseline )
        coalitions = self._coalitions(elements)
        for nodes_id in coalitions :
            for ex, nodes_values in enumerate(x[:, nodes_id]) :
                for k, feature in enumerate(nodes_id) :
                    contributions[feature, ex] += self._coalition_contribution(k, 
                                                                               ex,
                                                                               elements,
                                                                               int(feature),
                                                                               markovImpact, 
                                                                               nodes_id, 
                                                                               nodes_values, 
                                                                               cache)
        return contributions
