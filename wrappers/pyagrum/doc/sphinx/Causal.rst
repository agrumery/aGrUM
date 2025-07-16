Probabilistic causality
=======================

.. figure:: _static/causal.png
    :align: center
    :alt: Causality in pyAgrum

Causality in pyAgrum mainly consists in the ability to build a causal model from a (observational) Bayesian network and a set of latent variables and their relation with observation variables and in the ability to compute using do-calculus the causal impact in such a model.

Causality is a set of pure python3 scripts based on pyAgrum's tools.

Causality in pyAgrum primarily involves building a causal model—that is, constructing an (observational) Bayesian network along with a set of latent variables and defining their relationships with observed variables. It also includes the ability to compute causal effects in such models using do-calculus.

pyAgrum provides a set of tools to perform causal inference and estimate causal effects from data. This includes the ability to identify interventions, compute causal impacts, and evaluate the effects of interventions on observed variables.

The causality module consists of pure Python 3 scripts built on top of pyAgrum's tools.


.. note::
    In the figure above, `pyagrum.causal` module can use a LaTeX special arrow (:math:`\hookrightarrow`) to compactly represent an intervention. By default, it uses the classical "do" notation. You can change this behavior to anything you want by using the following configuration keys:

    .. code-block:: python

            pyagrum.config["causal","latex_do_prefix"]="\hookrightarrow("
            pyagrum.config["causal","latex_do_suffix"]=")"

Causal models and do-calculus
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. toctree::
   :maxdepth: 1

   CausalModel
   CausalInference

Causel Effect estimation
^^^^^^^^^^^^^^^^^^^^^^^^

.. toctree::
  :maxdepth: 1

  CausalEffectEstimation

Tutorials on causality
======================

* `Notebooks on causality in pyAgrum <notebooks.html#causal-bayesian-networks>`_.
* Some `implemented examples <https://webia.lip6.fr/~phw/aGrUM/BookOfWhy/>`_ from the `book of Why <http://bayes.cs.ucla.edu/WHY/>`_ from Judea Pearl and Dana Mackenzie.


Tools for causality
===================

.. toctree::
   :maxdepth: 1

   CausalNotebook
   CausalExceptions
   CausalAST
