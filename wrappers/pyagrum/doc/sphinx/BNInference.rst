Inference
^^^^^^^^^

Inference is the process that consists in computing new probabilistc information from a Bayesian network and some evidence. aGrUM/pyAgrum mainly focus on the computation of (joint) posterior for some variables of the Bayesian networks given soft or hard evidence that are the form of likelihoods on some variables.
Inference is a hard task (NP-complete). aGrUM/pyAgrum implements exact inference but also approximated inference that can converge slowly and (even) not exactly but thant can in many cases be useful for applications.

Exact Inference
---------------

Lazy Propagation
~~~~~~~~~~~~~~~~

Lazy Propagation is the main exact inference for classical Bayesian networks in aGrUM/pyagrum.

.. autoclass:: pyagrum.LazyPropagation
			:inherited-members:
			:exclude-members: setFindBarrenNodesType, setRelevantTensorsFinderType, setTriangulation


Shafer-Shenoy Inference
~~~~~~~~~~~~~~~~~~~~~~~

.. autoclass:: pyagrum.ShaferShenoyInference
			:exclude-members: setFindBarrenNodesType, setRelevantTensorsFinderType, setTriangulation

Variable Elimination
~~~~~~~~~~~~~~~~~~~~

.. autoclass:: pyagrum.VariableElimination
			:exclude-members: setFindBarrenNodesType, setRelevantTensorsFinderType, setTriangulation


Approximated Inference
----------------------

Loopy Belief Propagation
~~~~~~~~~~~~~~~~~~~~~~~~

.. autoclass:: pyagrum.LoopyBeliefPropagation
			:exclude-members: asIApproximationSchemeConfiguration

Sampling
~~~~~~~~

Gibbs Sampling for BN
+++++++++++++++++++++

.. autoclass:: pyagrum.GibbsSampling
			:exclude-members: asIApproximationSchemeConfiguration

Simple Monte Carlo Sampling for BN
++++++++++++++++++++++++++++++++++

.. autoclass:: pyagrum.MonteCarloSampling
			:exclude-members: asIApproximationSchemeConfiguration


Weighted Sampling for BN
++++++++++++++++++++++++

.. autoclass:: pyagrum.WeightedSampling
			:exclude-members: asIApproximationSchemeConfiguration

Importance Sampling for BN
++++++++++++++++++++++++++

.. autoclass:: pyagrum.ImportanceSampling
			:exclude-members: asIApproximationSchemeConfiguration

Loopy sampling
~~~~~~~~~~~~~~

Pure Loopy Gibbs Sampling
+++++++++++++++++++++++++

.. autoclass:: pyagrum.LoopyGibbsSampling
			:exclude-members: asIApproximationSchemeConfiguration

Loopy Monte Carlo Sampling
++++++++++++++++++++++++++

.. autoclass:: pyagrum.LoopyMonteCarloSampling
			:exclude-members: asIApproximationSchemeConfiguration

Loopy Weighted Sampling
+++++++++++++++++++++++

.. autoclass:: pyagrum.LoopyWeightedSampling
			:exclude-members: asIApproximationSchemeConfiguration

Loopy Importance Sampling
+++++++++++++++++++++++++

.. autoclass:: pyagrum.LoopyImportanceSampling
			:exclude-members: asIApproximationSchemeConfiguration
