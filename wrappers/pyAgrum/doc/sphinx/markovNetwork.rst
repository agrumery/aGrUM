Markov Network
================

.. figure:: _static/MarkovNet.png
    :align: center
    :alt: a Markov network as an unoriented graph and as a factor graph

A Markov network is a undirected probabilistic graphical model. It represents a joint distribution over a set of random variables. In pyAgrum, the variables are (for now) only discrete.

A Markov network uses a undirected graph to represent conditional independence in the joint distribution. These conditional independence allow to factorize the joint distribution, thereby allowing to compactly represent very large ones.

.. math::
   P(X_1,\cdots,X_n)\propto\prod_{i=1}^{n_c} \phi_i(C_i)

Where the :math:`\phi_i` are potentials over the :math:`n_c` cliques of the undirected graph.

Moreover, inference algorithms can also use this graph to speed up the computations.

**Tutorial**

* `Tutorial on Markov Network <https://lip6.fr/Pierre-Henri.Wuillemin/aGrUM/docs/current/notebooks/13_Models-MarkovNetwork.ipynb.html>`_

**Reference**

.. toctree::
   :maxdepth: 3

   MNModel

   MNInference

