# Imports
import pyagrum as gum
from pyagrum.explain._ShapleyValues import ShapleyValues
from pyagrum.explain._CustomShapleyCache import CustomShapleyCache
from pyagrum.explain._FIFOCache import FIFOCache
# Calculus
import numpy as np
import pandas as pd
# GL
import warnings

class CausalShapValues(ShapleyValues) :
    """
    The CausalShapValues class computes the Causal Shapley values for a given target node in a Bayesian Network.
    """
    def __init__(self, bn, target, background: tuple | None, sample_size=1000, logit=True) :
        """
        Parameters:
        ------
        bn : pyagrum.BayesNet
            The Bayesian Network.
        target : int | str
            The node id (or node name) of the target.
        background : Tuple(pandas.DataFrame, bool) | None
            A tuple containing a pandas DataFrame and a boolean indicating whether the DataFrame contains labels or positions.
        sample_size : int
            The size of the background sample to generate if `background` is None.
        logit : bool 
            If True, applies the logit transformation to the probabilities.

        Raises:
        ------
        TypeError : If bn is not a gum.BayesNet instance, background is not a tuple or target is not an integer or string.
        ValueError : If target is not a valid node id in the Bayesian Network or if sample_size is not a positive integer.
        """
        super().__init__(bn, target, logit)
        self._mb = self._markov_blanket()
        # Processing background data
        if background is None :
            if not isinstance(sample_size, int) :
                raise TypeError("When `data`=None, `sample_size` must be an integer, but got {}".format(type(sample_size)))
            else :
                if sample_size <= 1 :
                    raise ValueError("`sample_size` must be greater than 1, but got {}".format(sample_size))
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
        self._data = data
        self._N = len(self._data)
        # Calculating the baseline
        self.baseline = self._value(self._data, [i for i in range(self.M) if i != self.target], gum.LazyPropagation(self.bn), self._mb, FIFOCache(1000))
    
    def _markov_blanket(self) :
        # Retrieves the Markov blanket of the target node.
        mb = gum.MarkovBlanket(self.bn, self.target).nodes()
        mb.remove(self.target)
        return sorted(list(mb))
    
    def _outOfCoalition(self, tau: list[int])-> list[int] :
        # Returns the intersection of the Markov blanket and the complement of the coalition tau.
        return [i for i in self._mb if i not in tau]

    def _doCalculus(self, bn: gum.BayesNet, tau: list[int])-> gum.BayesNet :
        # Creates a new Bayesian Network by removing incoming arcs to the nodes in tau.
        doNetTemplate = gum.BayesNet(bn)
        for i in tau :
            parents = doNetTemplate.parents(i)
            for j in parents :
                doNetTemplate.eraseArc(j, i)
        return doNetTemplate

    def _chgCpt(self, doNetTemplate: gum.BayesNet, tau: list[int], alpha: list[int])-> None :
        # Changes the conditional probability tables (CPTs) of the nodes in tau to reflect the values in alpha.
        for i, j in zip(tau, alpha) :
            doNetTemplate.cpt(i).fillWith(0.)
            doNetTemplate.cpt(i)[int(j)] = 1.
    
    def _extract(self, tau: list[int], sigma: list[int], alpha: np.ndarray)-> np.ndarray :
        # Extracts the background data for the nodes in sigma given the values in alpha for the nodes in tau.
        idx = [i for i in range(self._N) if np.all(self._data[i, tau] == alpha)]
        subData = self._data[idx]
        _, udx = np.unique(subData[:, sigma], return_index=True, axis=0)
        return subData[udx]
    
    def _getProbability(self, doLazy: gum.LazyPropagation, sigma: list[int], beta: np.ndarray)-> np.ndarray :
        # Returns the probability that nodes in sigma take the values in beta.
        evidces = {key: int(val) for key, val in zip(sigma, beta)}
        doLazy.updateEvidence(evidces)
        return doLazy.evidenceProbability()
    
    def _value(self, interv: np.ndarray, elements: list[int], doLazy: gum.LazyPropagation, sigma: list[int], markovImpact: FIFOCache)-> np.ndarray :
        # Computes v = E[f(X) | do(X_S = x_s)]
        norm = 0.
        val = np.zeros(self.bn.variable(self.target).domainSize())
        for i in range(len(interv)) :
            posterior = markovImpact.get(tuple(interv[i, elements]), None)
            if posterior is None :
               evidces = {key: int(interv[i, key]) for key in elements}
               self.ie.updateEvidence(evidces)
               posterior = self.ie.posterior(self.target).toarray()
               markovImpact[tuple(interv[i, elements])] = posterior
            p = self._getProbability(doLazy, sigma, interv[i, sigma])
            norm += p
            val += posterior * p

        self.ie.eraseAllEvidence()
        return self.func(val / norm) if norm != 0. else val
    
    def _coalition_contribution(self, posterior_prob_with, posterior_prob_without, m, s) :
        # Computes the contribution of a coalition to the Shapley value.
        return (posterior_prob_with - posterior_prob_without) / self._invcoeff_shap(m, s)
        
    def _shap_1dim(self, x, elements):
        # Computes the Shapley values for a 1-dimensional input x (local explanation).
        contributions = np.zeros( (self.M, self.bn.variable(self.target).domainSize()) ) # Initializes contributions array.
        cache = CustomShapleyCache(5000)
        markovImpact = FIFOCache(1000)  
        cache.set(0, (), self.baseline) # Sets the baseline probability in the cache.
        coalitions = self._coalitions(elements) # Compute the coalitions

        for tau in coalitions :
            self.ie.eraseAllEvidence() # Clears all evidence from the inference engine.
            doNet = self._doCalculus(self.bn, tau) # Creates a new Bayesian Network to perform do-calculus.
            sigma = self._outOfCoalition(tau) # Extracts the nodes outside the coalition tau which are in the Markov blanket.
            alpha = x[tau] # Instanciation of tau
            if sigma != [] :
                self._chgCpt(doNet, tau, alpha) # Changes the conditional probability tables to perform do-calculus.
                doLazy = gum.LazyPropagation(doNet) # Creates a lazy propagation inference engine to compute partial join probabilities.
                doLazy.addTarget(self.target)   
                background_data = self._extract(tau, sigma, alpha)
                posterior_with = self._value(background_data, elements, doLazy, sigma, markovImpact)
            else :
                # If sigma is empty, v = P(Y | X_tau = alpha)
                evidces = {key: int(val) for key, val in zip(tau, alpha)}
                self.ie.updateEvidence(evidces)
                posterior_with = self.func( self.ie.posterior(self.target).toarray() )

            cache.set(0, tuple(tau), posterior_with)
            # Contribution of each feature
            for t in tau :
                key = tuple((f for f in tau if f != t))
                posterior_without = cache.get(0, key)
                contributions[t] += self._coalition_contribution(posterior_with,
                                                                    posterior_without,
                                                                    len(elements),
                                                                    len(tau) - 1)
        return contributions

    def _shap_ndim(self, x, elements) :
        contributions = np.zeros( (self.M, len(x), self.bn.variable(self.target).domainSize()) ) # Initializes contributions array.
        cache = CustomShapleyCache(5000) # Initializes the custom cache.
        markovImpact = FIFOCache(1000)
        cache.set(0, (), self.baseline) # Sets the baseline probability in the cache.
        coalitions = self._coalitions(elements) # Compute the coalitions

        for tau in coalitions :
            self.ie.eraseAllEvidence() # Clears all evidence from the inference engine.
            doNet = self._doCalculus(self.bn, tau) # Creates a new Bayesian Network to perform do-calculus.
            sigma = self._outOfCoalition(tau) # Extracts the nodes outside the coalition tau which are in the Markov blanket.

            for i in range(len(x)) : # Iterates over each example in x
                alpha = x[i, tau] # Instanciation of tau
                if sigma != [] :
                    self._chgCpt(doNet, tau, alpha) # Changes the conditional probability tables to perform do-calculus.
                    doLazy = gum.LazyPropagation(doNet) # Creates a lazy propagation inference engine to compute partial join probabilities.
                    doLazy.addTarget(self.target)
                    background_data = self._extract(tau, sigma, alpha)
                    posterior_with = self._value(background_data, elements, doLazy, sigma, markovImpact) # Compute the value for this coalition.

                else :
                    # If sigma is empty, v = P(Y | X_tau = alpha)
                    evidces = {key: int(val) for key, val in zip(tau, alpha)}
                    self.ie.updateEvidence(evidces)
                    posterior_with = self.func( self.ie.posterior(self.target).toarray() )

                cache.set(i, tuple(tau), posterior_with)
                # Contribution of each feature
                for t in tau :
                    key = tuple((f for f in tau if f != t))
                    posterior_without = cache.get(i, key) if len(key) > 0 else cache.get(0, ())
                    contributions[t, i] += self._coalition_contribution(posterior_with,
                                                                        posterior_without,
                                                                        len(elements),
                                                                        len(tau) - 1)
        return contributions
