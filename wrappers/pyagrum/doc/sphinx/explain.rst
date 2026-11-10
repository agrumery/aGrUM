pyagrum.explain
===============

The purpose of ``pyagrum.explain`` is to give tools to explain and interpret the structure and
parameters of a Bayesian network.

Dealing with independence
-------------------------

.. figure:: _static/independenceList.png
    :align: center
    :alt: independenceList in pyAgrum


.. pyagrum-functions::

   pyagrum.explain.independenceListForPairs


Dealing with mutual information and entropy
-------------------------------------------

.. figure:: _static/showInformation.png
    :align: center
    :alt: showing entropy and mutual informations in pyAgrum


.. pyagrum-functions::

   pyagrum.explain.getInformation
   pyagrum.explain.getInformationGraph
   pyagrum.explain.showInformation

Dealing with Shap Values and Shall Values
-----------------------------------------

.. figure:: _static/ShapValue.png
    :align: center
    :alt: Shap-Values in pyAgrum

All attribution methods return an :class:`~pyagrum.explain.Explanation` object, which maps
feature names to their individual attribution values and carries auxiliary data from the
computation.

.. pyagrum-class:: pyagrum.explain.Explanation
   :no-members:
   :no-inherited-members:

Shap Values
~~~~~~~~~~~

Shap (SHapley Additive exPlanations) values decompose a model prediction into the additive
contributions of each feature, based on the Shapley value from cooperative game theory.
Three variants are available depending on how feature interactions are handled:

.. pyagrum-class:: pyagrum.explain.MarginalShapValues
    :members:
    :undoc-members:
    :show-inheritance:

.. pyagrum-class:: pyagrum.explain.ConditionalShapValues
    :members:
    :undoc-members:
    :show-inheritance:

The causal Shapley values are computed following :cite:t:`heskes2020causal`.

.. pyagrum-class:: pyagrum.explain.CausalShapValues
    :members:
    :undoc-members:
    :show-inheritance:

Shall Values
~~~~~~~~~~~~

Shall (SHapley Additive Log-Likelihood) values are a pyAgrum-specific explanation method.
Where Shap values attribute a *prediction*, Shall values attribute the *log-likelihood of a
baseline observation*: they decompose :math:`\log P(e)` — the log-probability of an evidence
:math:`e` — into the additive contribution of each variable. This makes them particularly
suited for explaining *why* a given observation is more or less probable under the model,
rather than explaining a prediction output.

The same three variants (marginal, conditional, causal) are available:

.. pyagrum-class:: pyagrum.explain.MarginalShallValues
    :members:
    :undoc-members:
    :show-inheritance:

.. pyagrum-class:: pyagrum.explain.ConditionalShallValues
    :members:
    :undoc-members:
    :show-inheritance:

.. pyagrum-class:: pyagrum.explain.CausalShallValues
    :members:
    :undoc-members:
    :show-inheritance:

Dealing with generalized Markov Blankets
----------------------------------------

A structural property of Bayesian networks is the Markov boundary of a node. A Markov blanket of a node is a set of nodes that renders the node independent of all other nodes in the network. The Markov boundary is the closest Markov blanket. A Markov boundary of a node is composed of its parents, its children, and the parents of its children. More generally, one can define the generalized :math:`k`-Markov blanket of a node as the union of the markov blanket of the nodes of its :math:`(k-1)`-Markov blanket. So, if a node belongs to the :math:`k`-Markov blanket of the node :math:`X`, :math:`k` is a kind of measure of its proximity to :math:`X`.

.. figure:: _static/generalizedMarkovBlanket.png
    :align: center
    :alt: Generalized Markov Blanket in pyAgrum

.. pyagrum-functions::

   pyagrum.explain.generalizedMarkovBlanket

.. bibliography::
    :filter: docname in docnames
