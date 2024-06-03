pyAgrum.lib.explain
===================

The purpose of ``pyAgrum.lib.explain`` is to give tools to explain and interpret the structure and parameters of a Bayesian network.

Dealing with independence
-------------------------

.. autofunction:: pyAgrum.lib.explain.independenceListForPairs


Dealing with mutual information and entropy
-------------------------------------------

.. autofunction:: pyAgrum.lib.explain.getInformation
.. autofunction:: pyAgrum.lib.explain.showInformation

Dealing with ShapValues
-----------------------

.. figure:: _static/ShapValue.png
    :align: center
    :alt: Shap-Values in pyAgrum

.. autoclass:: pyAgrum.lib.explain.ShapValues
    :members:
    :undoc-members:
    :show-inheritance:
    :noindex:

Dealing with generalized Markov Blankets
----------------------------------------

A structural property of Bayesian networks is the Markov boundary of a node. A Markov blanket of a node is a set of nodes that renders the node independent of all other nodes in the network. The Markov boundary is the closest Markov blanket. A Markov boundary of a node is composed of its parents, its children, and the parents of its children. More generally, one can define the generalized :math:`k`-Markov blanket of a node as the union of the markov blanket of the nodes of its :math:`(k-1)`-Markov blanket. So, if a node belongs to the :math:`k`-Markov blanket of the node :math:`X`, :math:`k` is a kind of measure of its proximity to :math:`X`.

.. figure:: _static/generalizedMarkovBlanket.png
    :align: center
    :alt: Generalized Markov Blanket in pyAgrum

.. autofunction:: pyAgrum.lib.explain.generalizedMarkovBlanket
