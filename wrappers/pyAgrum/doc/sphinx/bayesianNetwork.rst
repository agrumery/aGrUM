Bayesian Network
================

The Bayesian Network is the main object of pyAgrum. A bayesian network is a probabilistic graphical model. It represents a joint distribution over a set of random variables. In pyAgrum, the variable are (for now) only discrete. A bayesian network use a directed acyclic graph (DAG) to represent conditional indepencies in the joint distribution. These conditional indepencies allow to factorize the joint distribution and then allow to compactly represent very large ones. Moreover, inference algorithms can also use this graph to speed up the computations. Finally, the bayesian networks can be learned from data.

Model
-----

.. autoclass:: pyAgrum.BayesNet
			:exclude-members: setProperty, property, propertyWithDefault

.. autoclass:: pyAgrum.pyAgrum.BayesNet_double
			:exclude-members: setProperty, property, propertyWithDefault

Inference
---------

.. autoclass:: pyAgrum.LazyPropagation

.. autoclass:: pyAgrum.pyAgrum.LazyPropagation_double

.. autoclass:: pyAgrum.GibbsInference

.. autoclass:: pyAgrum.pyAgrum.GibbsInference_double

Learning
--------

.. autoclass:: pyAgrum.BNLearner

.. autoclass:: pyAgrum.pyAgrum.BNLearner_double