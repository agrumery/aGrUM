Bayesian Network
================

The Bayesian Network is the main object of pyAgrum. A bayesian network is a probabilistic graphical model. It represents a joint distribution over a set of random variables. In pyAgrum, the variable are (for now) only discrete. A bayesian network use a directed acyclic graph (DAG) to represent conditional indepencies in the joint distribution. These conditional indepencies allow to factorize the joint distribution and then allow to compactly represent very large ones. Moreover, inference algorithms can also use this graph to speed up the computations. Finally, the bayesian networks can be learned from data.

Model
-----

.. autoclass:: pyAgrum.BayesNet
			:exclude-members: setProperty, property, propertyWithDefault

.. autoclass:: pyAgrum.pyAgrum.BayesNet_double
			:exclude-members: setProperty, property, propertyWithDefault

Exact Inference
---------------

Lazy Propagation
~~~~~~~~~~~~~~~~

.. autoclass:: pyAgrum.LazyPropagation
			:inherited-members:
			:exclude-members: setFindBarrenNodesType, setRelevantPotentialsFinderType, setTriangulation

.. autoclass:: pyAgrum.pyAgrum.LazyPropagation_double
			:exclude-members: setFindBarrenNodesType, setRelevantPotentialsFinderType, setTriangulation

Shafer Shenoy Inference
~~~~~~~~~~~~~~~~~~~~~~~

.. autoclass:: pyAgrum.ShaferShenoyInference
			:exclude-members: setFindBarrenNodesType, setRelevantPotentialsFinderType, setTriangulation

.. autoclass:: pyAgrum.pyAgrum.ShaferShenoyInference_double
			:exclude-members: setFindBarrenNodesType, setRelevantPotentialsFinderType, setTriangulation

Variable Elimination
~~~~~~~~~~~~~~~~~~~~

.. autoclass:: pyAgrum.VariableElimination
			:exclude-members: setFindBarrenNodesType, setRelevantPotentialsFinderType, setTriangulation

.. autoclass:: pyAgrum.pyAgrum.VariableElimination_double
			:exclude-members: setFindBarrenNodesType, setRelevantPotentialsFinderType, setTriangulation

Approximate Inference
---------------------

Loopy Belief Propagation
~~~~~~~~~~~~~~~~~~~~~~~~

.. autoclass:: pyAgrum.LoopyBeliefPropagation
			:exclude-members: asIApproximationSchemeConfiguration

.. autoclass:: pyAgrum.pyAgrum.LoopyBeliefPropagation_double
			:exclude-members: asIApproximationSchemeConfiguration

Gibbs Sampling
~~~~~~~~~~~~~~

.. autoclass:: pyAgrum.GibbsSampling
			:exclude-members: asIApproximationSchemeConfiguration

.. autoclass:: pyAgrum.pyAgrum.GibbsSampling_double
			:exclude-members: asIApproximationSchemeConfiguration

Monte Carlo Sampling
~~~~~~~~~~~~~~~~~~~~

.. autoclass:: pyAgrum.MonteCarloSampling
			:exclude-members: asIApproximationSchemeConfiguration

.. autoclass:: pyAgrum.pyAgrum.MonteCarloSampling_double
			:exclude-members: asIApproximationSchemeConfiguration

Weighted Sampling
~~~~~~~~~~~~~~~~~

.. autoclass:: pyAgrum.WeightedSampling
			:exclude-members: asIApproximationSchemeConfiguration

.. autoclass:: pyAgrum.pyAgrum.WeightedSampling_double
			:exclude-members: asIApproximationSchemeConfiguration

Importance Sampling
~~~~~~~~~~~~~~~~~~~

.. autoclass:: pyAgrum.ImportanceSampling
			:exclude-members: asIApproximationSchemeConfiguration

.. autoclass:: pyAgrum.pyAgrum.ImportanceSampling_double
			:exclude-members: asIApproximationSchemeConfiguration

Loopy version
-------------

Loopy Gibbs Sampling
~~~~~~~~~~~~~~~~~~~~

.. autoclass:: pyAgrum.LoopyGibbsSampling
			:exclude-members: asIApproximationSchemeConfiguration

.. autoclass:: pyAgrum.pyAgrum.LoopyGibbsSampling_double
			:exclude-members: asIApproximationSchemeConfiguration

Loopy Monte Carlo Sampling
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. autoclass:: pyAgrum.LoopyMonteCarloSampling
			:exclude-members: asIApproximationSchemeConfiguration

.. autoclass:: pyAgrum.pyAgrum.LoopyMonteCarloSampling_double
			:exclude-members: asIApproximationSchemeConfiguration

Loopy Weighted Sampling
~~~~~~~~~~~~~~~~~~~~~~~

.. autoclass:: pyAgrum.LoopyWeightedSampling
			:exclude-members: asIApproximationSchemeConfiguration

.. autoclass:: pyAgrum.pyAgrum.LoopyWeightedSampling_double
			:exclude-members: asIApproximationSchemeConfiguration

Loopy Importance Sampling
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. autoclass:: pyAgrum.LoopyImportanceSampling
			:exclude-members: asIApproximationSchemeConfiguration

.. autoclass:: pyAgrum.pyAgrum.LoopyImportanceSampling_double
			:exclude-members: asIApproximationSchemeConfiguration

Learning
--------

.. autoclass:: pyAgrum.BNLearner
			:exclude-members: asIApproximationSchemeConfiguration

.. autoclass:: pyAgrum.pyAgrum.BNLearner_double
			:exclude-members: asIApproximationSchemeConfiguration