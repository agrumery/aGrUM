Bayesian network
================

.. figure:: _static/BayesNet.png
    :align: center
    :alt: a Bayesian network (alarm)

The Bayesian network is the main graphical model of pyAgrum. A Bayesian network is a
directed probabilistic graphical model based on a DAG. It represents a joint distribution over a set of random variables. In pyAgrum, the variables are (for now) only discrete.

A Bayesian network uses a directed acyclic graph (DAG) to represent conditional
indepencies in the joint distribution. These conditional indepencies allow to
factorize the joint distribution, thereby allowing to compactly represent very
large ones. Moreover, inference algorithms can also use this graph to speed up
the computations. Finally, the Bayesian networks can be learnt from data.

**Tutorial**

* `Tutorial on Bayesian network <http://webia.lip6.fr/~phw/aGrUM/docs/last/notebooks/01-tutorial.ipynb.html>`_

**Reference**

.. toctree::
   :maxdepth: 3

   BNModel

   BNTools

   BNInference

   BNLearning


