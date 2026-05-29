Learning a CLG
^^^^^^^^^^^^^^

One of the main features of this library is the possibility to learn a CLG.

More precisely what can be learned is :
    - The dependency graph of a CLG
    - The parameters of a CLG: the mu and sigma of each variable, the coefficients of the arcs

**Learning the graph**

To learn the graph of a CLG (ie the dependence between variables) we use a modified PC algorithm
based on :cite:t:`colombo2014order`.

The independence test used is based on :cite:t:`simionato2022bounding`.

.. autoclass:: pyagrum.clg.learning.CLGLearner

.. bibliography::
    :filter: docname in docnames
