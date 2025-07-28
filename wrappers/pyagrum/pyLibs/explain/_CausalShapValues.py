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

        self._data = data
        self._N = len(self._data)
        # Calculating the baseline
        doLazy = gum.LazyPropagation(self.bn)
        self.baseline = self.func( self.ie.posterior(self.target).toarray() )
    
    def _markov_blanket(self) :
        mb = gum.MarkovBlanket(self.bn, self.target).nodes()
        mb.remove(self.target)
        return sorted(list(mb))
    
    def _outOfCoalition(self, tau: list[int])-> list[int] :
        return [i for i in self._mb if i not in tau]

    def _doCalculus(self, bn: gum.BayesNet, tau: list[int])-> gum.BayesNet :
        doNetTemplate = gum.BayesNet(bn)
        for i in tau :
            parents = doNetTemplate.parents(i)
            for j in parents :
                doNetTemplate.eraseArc(j, i)
        return doNetTemplate

    def _chgCpt(self, doNetTemplate: gum.BayesNet, tau: list[int], alpha: list[int])-> None :
        for i, j in zip(tau, alpha) :
            doNetTemplate.cpt(i).fillWith(0.)
            doNetTemplate.cpt(i)[int(j)] = 1.
    
    def _extract(self, tau: list[int], sigma: list[int], alpha: np.ndarray)-> np.ndarray :
        idx = [i for i in range(self._N) if np.all(self._data[i, tau] == alpha)]
        subData = self._data[idx]
        _, udx = np.unique(subData[:, sigma], return_index=True, axis=0)
        return subData[udx]
    
    def _getProbability(self, doLazy: gum.LazyPropagation, sigma: list[int], beta: np.ndarray)-> np.ndarray :
        evidces = {key: int(val) for key, val in zip(sigma, beta)}
        doLazy.updateEvidence(evidces)
        return doLazy.evidenceProbability()
    
    def _value(self, interv: np.ndarray, elements: list[int], doLazy: gum.LazyPropagation, sigma: list[int], markovImpact: FIFOCache)-> np.ndarray :
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
        return (posterior_prob_with - posterior_prob_without) / self._invcoeff_shap(m, s)
        
    def _shap_1dim(self, x, elements):
        contributions = np.zeros( (self.M, self.bn.variable(self.target).domainSize()) )

        # Caches
        cache = CustomShapleyCache(5000)
        markovImpact = FIFOCache(1000)
        # Sets the baseline probability in the cache.
        cache.set(0, (), self.baseline)
        # Compute the coalitions
        coalitions = self._coalitions(elements)

        for tau in coalitions :
            self.ie.eraseAllEvidence()

            doNet = self._doCalculus(self.bn, tau)
            sigma = self._outOfCoalition(tau)
            # Instanciation of tau
            alpha = x[tau]
            if sigma != [] :
                self._chgCpt(doNet, tau, alpha)
                doLazy = gum.LazyPropagation(doNet)
                doLazy.addTarget(self.target)
                background_data = self._extract(tau, sigma, alpha)
                # Compute the value for this coalition.
                if tau == [0, 1, 3, 5, 6] :
                    print(background_data)
                posterior_with = self._value(background_data, elements, doLazy, sigma, markovImpact)
            else :
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
        # Initialisation
        contributions = np.zeros( (self.M, len(x), self.bn.variable(self.target).domainSize()) )

        # Caches
        cache = CustomShapleyCache(5000)
        markovImpact = FIFOCache(1000)
        # Sets the baseline probability in the cache.
        cache.set(0, (), self.baseline)
        # Compute the coalitions
        coalitions = self._coalitions(elements)

        for tau in coalitions :
            self.ie.eraseAllEvidence()
            doNet = self._doCalculus(self.bn, tau)
            sigma = self._outOfCoalition(tau)

            for i in range(len(x)) :
                alpha = x[i, tau]
                if sigma != [] :
                    # Instanciation of tau
                    self._chgCpt(doNet, tau, alpha)
                    doLazy = gum.LazyPropagation(doNet)
                    doLazy.addTarget(self.target)
                    background_data = self._extract(tau, sigma, alpha)
                    # Compute the value for this coalition.
                    posterior_with = self._value(background_data, elements, doLazy, sigma, markovImpact)
                else :
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

if __name__ == '__main__' :
    # Building the Bayesian Network
    bn = gum.fastBN('X->Y->Z')
    bn.cpt('X')[:] = [0.2, 0.8]
    bn.cpt('Y')[:] = [[0.3, 0.7],
                    [0.7, 0.3]]
    bn.cpt('Z')[:] = [[0.3, 0.7],
                    [0.9, 0.1]]
    # Data
    data = gum.generateSample(bn, 100, with_labels=True)[0].astype('U10')
    background_data = data.sample(10)

    # The explainer
    explainer = CausalShapValues(bn, 'Y', background_data)

    # The explanation
    expl = explainer.compute(data, with_labels=True)
