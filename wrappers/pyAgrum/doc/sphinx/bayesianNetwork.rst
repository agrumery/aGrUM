Bayesian Network
================

The Bayesian Network is the main object of pyAgrum. A Bayesian network is a probabilistic graphical model. It represents a joint distribution over a set of random variables. In pyAgrum, the variables are (for now) only discrete. A Bayesian network uses a directed acyclic graph (DAG) to represent conditional indepencies in the joint distribution. These conditional indepencies allow to factorize the joint distribution, thereby allowing to compactly represent very large ones. Moreover, inference algorithms can also use this graph to speed up the computations. Finally, the Bayesian networks can be learnt from data.

Model
-----

.. autoclass:: pyAgrum.BayesNet
			:exclude-members: setProperty, property, propertyWithDefault

Exact Inference
---------------

Lazy Propagation
~~~~~~~~~~~~~~~~

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


Approximate Inference
---------------------

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

Learning
--------

.. autoclass:: pyAgrum.BNLearner
			:exclude-members: asIApproximationSchemeConfiguration

