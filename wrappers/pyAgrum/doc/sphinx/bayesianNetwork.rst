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

.. autoclass:: pyAgrum.LazyPropagation

.. autoclass:: pyAgrum.pyAgrum.LazyPropagation_double

.. autoclass:: pyAgrum.ShaferShenoyInference

.. autoclass:: pyAgrum.pyAgrum.ShaferShenoyInference_double

.. autoclass:: pyAgrum.VariableElimination

.. autoclass:: pyAgrum.VariableElimination_double

Approximate Inference
---------------------

.. autoclass:: pyAgrum.LoopyBeliefPropagation

.. autoclass:: pyAgrum.pyAgrum.LoopyBeliefPropagation_double

.. autoclass:: pyAgrum.GibbsSampling

.. autoclass:: pyAgrum.pyAgrum.GibbsSampling_double

.. autoclass:: pyAgrum.MonteCarloSampling

.. autoclass:: pyAgrum.pyAgrum.MonteCarloSampling_double

.. autoclass:: pyAgrum.WeightedSampling

.. autoclass:: pyAgrum.pyAgrum.WeightedSampling_double

.. autoclass:: pyAgrum.ImportanceSampling

.. autoclass:: pyAgrum.pyAgrum.ImportanceSampling_double

Loopy version
-------------

.. autoclass:: pyAgrum.LoopyGibbsSampling

.. autoclass:: pyAgrum.pyAgrum.LoopyGibbsSampling_double

.. autoclass:: pyAgrum.LoopyMonteCarloSampling

.. autoclass:: pyAgrum.pyAgrum.LoopyMonteCarloSampling_double

.. autoclass:: pyAgrum.LoopyWeightedSampling

.. autoclass:: pyAgrum.pyAgrum.LoopyWeightedSampling_double

.. autoclass:: pyAgrum.LoopyImportanceSampling

.. autoclass:: pyAgrum.pyAgrum.LoopyImportanceSampling_double

Learning
--------

.. autoclass:: pyAgrum.BNLearner

.. autoclass:: pyAgrum.pyAgrum.BNLearner_double