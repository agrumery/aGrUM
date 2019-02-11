Inference
---------
Inference is the process that consists in computing new probabilistc information from a Bayesian network and some evidence. aGrUM/pyAgrum mainly focus and the computation of (joint) posterior for some variables of the Bayesian networks given soft or hard evidence that are the form of likelihoods on some variables.
Inference is a hard task (NP-complete). aGrUM/pyAgrum implements exact inference but also approximated inference that can converge slowly and (even) not exactly but thant can in many cases be useful for applications.

Exact Inference
---------------

Lazy Propagation
~~~~~~~~~~~~~~~~

Lazy Propagation is the main exact inference for classical Bayesian networks in aGrUM/pyAgrum.

.. autoclass:: pyAgrum.LazyPropagation
			:inherited-members:
			:exclude-members: setFindBarrenNodesType, setRelevantPotentialsFinderType, setTriangulation


Shafer Shenoy Inference
~~~~~~~~~~~~~~~~~~~~~~~

.. autoclass:: pyAgrum.ShaferShenoyInference
			:exclude-members: setFindBarrenNodesType, setRelevantPotentialsFinderType, setTriangulation

Variable Elimination
~~~~~~~~~~~~~~~~~~~~

.. autoclass:: pyAgrum.VariableElimination
			:exclude-members: setFindBarrenNodesType, setRelevantPotentialsFinderType, setTriangulation


Approximated Inference
----------------------

Loopy Belief Propagation
~~~~~~~~~~~~~~~~~~~~~~~~

.. autoclass:: pyAgrum.LoopyBeliefPropagation
			:exclude-members: asIApproximationSchemeConfiguration

Sampling
~~~~~~~~

Gibbs Sampling
++++++++++++++

.. autoclass:: pyAgrum.GibbsSampling
			:exclude-members: asIApproximationSchemeConfiguration

Monte Carlo Sampling
++++++++++++++++++++

.. autoclass:: pyAgrum.MonteCarloSampling
			:exclude-members: asIApproximationSchemeConfiguration


Weighted Sampling
+++++++++++++++++

.. autoclass:: pyAgrum.WeightedSampling
			:exclude-members: asIApproximationSchemeConfiguration

Importance Sampling
+++++++++++++++++++

.. autoclass:: pyAgrum.ImportanceSampling
			:exclude-members: asIApproximationSchemeConfiguration

Loopy sampling
~~~~~~~~~~~~~~

Loopy Gibbs Sampling
++++++++++++++++++++

.. autoclass:: pyAgrum.LoopyGibbsSampling
			:exclude-members: asIApproximationSchemeConfiguration

Loopy Monte Carlo Sampling
++++++++++++++++++++++++++

.. autoclass:: pyAgrum.LoopyMonteCarloSampling
			:exclude-members: asIApproximationSchemeConfiguration

Loopy Weighted Sampling
+++++++++++++++++++++++

.. autoclass:: pyAgrum.LoopyWeightedSampling
			:exclude-members: asIApproximationSchemeConfiguration

Loopy Importance Sampling
+++++++++++++++++++++++++

.. autoclass:: pyAgrum.LoopyImportanceSampling
			:exclude-members: asIApproximationSchemeConfiguration
