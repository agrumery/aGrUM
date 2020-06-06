Markov Network
================

.. figure:: _static/MarkovNet.png
    :align: center
    :alt: a Markov network as an unoriented graph and as a factor graph

A Markov network is a undirected probabilistic graphical model. It represents a joint distribution over a set of random variables. In pyAgrum, the variables are (for now) only discrete.

A Markov network uses a undirected graph to represent conditional
indepencies in the joint distribution. These conditional indepencies allow to factorize the joint distribution, thereby allowing to compactly represent very large ones. Moreover, inference algorithms can also use this graph to speed up the computations.

**Tutorial**

* `Tutorial on Markov Network <http://webia.lip6.fr/~phw/aGrUM/docs/current/notebooks/35-MarkovNetwork.ipynb.html>`_

**Reference**

.. toctree::
   :maxdepth: 3

   MNModel

   MNInference

