Inference
^^^^^^^^^

Inference is the process that consists in computing new probabilistc information from a Bayesian network and some evidence. aGrUM/pyAgrum mainly focus on the computation of (joint) posterior for some variables of the Bayesian networks given soft or hard evidence that are the form of likelihoods on some variables.
Inference is a hard task (NP-complete). aGrUM/pyAgrum implements exact inference but also approximated inference that can converge slowly and (even) not exactly but thant can in many cases be useful for applications.

Exact Inference
---------------

Lazy Propagation
~~~~~~~~~~~~~~~~

Lazy Propagation is the main exact inference for classical Bayesian networks in aGrUM/pyAgrum.

.. autoclass:: pyAgrum.bn.LazyPropagation
			:inherited-members:
			:exclude-members: setFindBarrenNodesType, setRelevantTensorsFinderType, setTriangulation


Shafer-Shenoy Inference
~~~~~~~~~~~~~~~~~~~~~~~

.. autoclass:: pyAgrum.bn.ShaferShenoyInference
			:exclude-members: setFindBarrenNodesType, setRelevantTensorsFinderType, setTriangulation

Variable Elimination
~~~~~~~~~~~~~~~~~~~~

.. autoclass:: pyAgrum.bn.VariableElimination
			:exclude-members: setFindBarrenNodesType, setRelevantTensorsFinderType, setTriangulation


Approximated Inference
----------------------

Loopy Belief Propagation
~~~~~~~~~~~~~~~~~~~~~~~~

.. autoclass:: pyAgrum.bn.LoopyBeliefPropagation
			:exclude-members: asIApproximationSchemeConfiguration

Sampling
~~~~~~~~

Gibbs Sampling for BN
+++++++++++++++++++++

.. autoclass:: pyAgrum.bn.GibbsSampling
			:exclude-members: asIApproximationSchemeConfiguration

Simple Monte Carlo Sampling for BN
++++++++++++++++++++++++++++++++++

.. autoclass:: pyAgrum.bn.MonteCarloSampling
			:exclude-members: asIApproximationSchemeConfiguration


Weighted Sampling for BN
++++++++++++++++++++++++

.. autoclass:: pyAgrum.bn.WeightedSampling
			:exclude-members: asIApproximationSchemeConfiguration

Importance Sampling for BN
++++++++++++++++++++++++++

.. autoclass:: pyAgrum.bn.ImportanceSampling
			:exclude-members: asIApproximationSchemeConfiguration

Loopy sampling
~~~~~~~~~~~~~~

Pure Loopy Gibbs Sampling
+++++++++++++++++++++++++

.. autoclass:: pyAgrum.bn.LoopyGibbsSampling
			:exclude-members: asIApproximationSchemeConfiguration

Loopy Monte Carlo Sampling
++++++++++++++++++++++++++

.. autoclass:: pyAgrum.bn.LoopyMonteCarloSampling
			:exclude-members: asIApproximationSchemeConfiguration

Loopy Weighted Sampling
+++++++++++++++++++++++

.. autoclass:: pyAgrum.bn.LoopyWeightedSampling
			:exclude-members: asIApproximationSchemeConfiguration

Loopy Importance Sampling
+++++++++++++++++++++++++

.. autoclass:: pyAgrum.bn.LoopyImportanceSampling
			:exclude-members: asIApproximationSchemeConfiguration
