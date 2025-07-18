Model for Bayesian Network
===========================

.. figure:: _static/BayesNet.png
    :align: center
    :alt: a Bayesian network (alarm)

The Bayesian network is the main graphical model of pyagrum. A Bayesian network is a
directed probabilistic graphical model based on a DAG.
It represents a joint distribution over a set of random variables.
In pyAgrum, the variables are (for now) only discrete.

A Bayesian network uses a directed acyclic graph (DAG) to represent conditional
independence in the joint distribution. These conditional independence allow to
factorize the joint distribution, thereby allowing to compactly represent very
large ones.

.. math::
   P(X_1,\cdots,X_n)=\prod_{i=1}^n P(X_i | Parents(X_i))

Moreover, inference algorithms can also use this graph to speed up
the computations. Finally, the Bayesian networks can be learnt from data.

.. autoclass:: pyagrum.BayesNet
