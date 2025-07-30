# Imports
import pyagrum as gum
# Calculations
import numpy as np

class CausalComputation() :

    @staticmethod
    def _outOfCoalition(tau: list[int], inter: list[int])-> list[int] :
        # Returns the intersection of the parameter inter and the complement of the coalition tau.
        return [i for i in inter if i not in tau]
    
    @staticmethod
    def _doCalculus(bn: gum.BayesNet, tau: list[int])-> gum.BayesNet :
        # Creates a new Bayesian Network by removing incoming arcs to the nodes in tau.
        doNetTemplate = gum.BayesNet(bn)
        for i in tau :
            parents = doNetTemplate.parents(i)
            for j in parents :
                doNetTemplate.eraseArc(j, i)
        return doNetTemplate

    @staticmethod
    def _chgCpt(doNetTemplate: gum.BayesNet, tau: list[int], alpha: list[int])-> None :
        # Changes the conditional probability tables (CPTs) of the nodes in tau to reflect the values in alpha.
        for i, j in zip(tau, alpha) :
            doNetTemplate.cpt(i).fillWith(0.)
            doNetTemplate.cpt(i)[int(j)] = 1.
        
    @staticmethod
    def _weight(evidces: dict[int, int], count: int, doLazy: gum.LazyPropagation)-> np.ndarray :
        # Returns the evidces probability.
        # The signature must be : Dict[int, int], int, **kwargs
        doLazy.updateEvidence(evidces)
        return doLazy.evidenceProbability() * count


