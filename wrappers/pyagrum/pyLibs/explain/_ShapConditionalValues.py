# Imports
from pyagrum.explain._ShapleyValues import ShapleyValues
from pyagrum.explain._CustomShapleyCache import CustomShapleyCache
# Calculations
import numpy as np
    
class ConditionalShapValues(ShapleyValues) :
    """
    The ConditionalShapValues class computes the conditional Shapley values for a given target node in a Bayesian Network.
    """
    def __init__(self, bn, target, logit=True) :
        """
        Parameters:
        ------
        bn : pyagrum.BayesNet
            The Bayesian Network.
        target : int | str
            The node id (or node name) of the target.
        logit : bool 
            If True, applies the logit transformation to the probabilities.

        Raises:
        ------
        TypeError : If bn is not a gum.BayesNet or target is not an integer or string.
        ValueError : If target is not a valid node id in the Bayesian Network.
        """
        super().__init__(bn, target, logit) # Initializes the ShapleyValues class.
        self.baseline = self.func( self.ie.posterior(self.target).toarray() ) # Sets the baseline probability.

    def _coalition_contribution(self, k, ex, _M, feature, nodes_id, nodes_vals, cache) :
        key1, key2, key_m = cache.generate_keys(self.bn, self.target, feature, nodes_id)
        if k == 0 :
            self.ie.eraseAllEvidence()
            if( len(key1) == len(key_m) ) :
                # If all nodes impact the target.
                # In this case, we are obliged to calculate the posterior probability.
                evidces = {key: int(value) for key, value in zip(nodes_id, nodes_vals)}
                self.ie.updateEvidence(evidces) # Updates the evidence in the inference engine.
                cache.set(ex, key1, self.func(self.ie.posterior(self.target).toarray())) # Sets the posterior probability in the cache.
            else :
                # Otherwise, we have already calculated the posterior probability.
                if len(key1) > 1 :
                    cache.set(ex, key1, cache.get(ex, key_m))
                else :
                    cache.set(ex, key1, cache.get(-1, ''))
        posterior_prob_with = cache.get(ex, key1)
        posterior_prob_without = cache.get(ex, key2) if len( key1 ) > 1 else cache.get(-1, '')
        return (posterior_prob_with - posterior_prob_without) / self._invcoeff_shap(_M, len(nodes_id) - 1)

    def _shap_1dim(self, x, elements)-> np.ndarray :
        contributions = np.zeros( (self.M, self.bn.variable(self.target).domainSize()) ) # Initializes contributions array.
        self.ie.eraseAllEvidence() # Clears all evidence from the inference engine.
        cache = CustomShapleyCache(5000) # Initializes the custom cache.
        cache.set(-1, '', self.baseline) # Sets the baseline probability in the cache.

        coalitions = self._coalitions(elements) # Generates coalitions.
        for nodes_id in coalitions :
            nodes_vals = x[nodes_id] # Gets the values of the nodes in the coalition.
            for k, feature in enumerate(nodes_id) :
                # Accumulates the contribution for each feature.
                contributions[feature] += self._coalition_contribution(k,
                                                                       0,
                                                                       len(elements),
                                                                       int(feature),
                                                                       nodes_id,
                                                                       nodes_vals,
                                                                       cache)
        return contributions

    def _shap_ndim(self, x, elements)-> np.ndarray :
        contributions = np.zeros( (self.M, len(x), self.bn.variable(self.target).domainSize()) ) # Initializes contributions array.
        self.ie.eraseAllEvidence() # Clears all evidence from the inference engine.
        cache = CustomShapleyCache(5000) # Initializes the custom cache.
        cache.set(-1, '', self.baseline) # Sets the baseline probability in the cache.

        coalitions = self._coalitions(elements) # Generates coalitions.
        for nodes_id in coalitions :
            data_vals = x[:, nodes_id] # Gets the values of the nodes in the coalition.
            for ex, nodes_vals in enumerate(data_vals) :
                for k, feature in enumerate( nodes_id ) :
                    contributions[feature, ex] += self._coalition_contribution(k,
                                                                               ex,
                                                                               len(elements),
                                                                               int(feature),
                                                                               nodes_id,
                                                                               nodes_vals,
                                                                               cache)
        return contributions