Causality
=========

.. figure:: _static/causal.png
    :align: center
    :alt: Causality in pyAgrum

Causality in pyAgrum mainly consists in the ability to build a causal model, i.e. a (observational) Bayesian network and a set of latent variables and their relation with observation variables and in the abilidy to compute using do-calculus the causal impact in such a model.

Causality is a set of pure python3 scripts based on pyAgrum's tools.

.. note::
    As it can be seen in the figure above, `pyAgrum.causal` module uses a LaTeX special arrow (:math:`\hookrightarrow`) to compactly represent an intervention. If you prefer the classical "do" notation, you can change this behavior by using:

    .. code::

            gum.config["causal","latex_do_prefix"]="do("
            gum.config["causal","latex_do_suffix"]=")"


**Tutorials**

* `Notebooks on causality in pyAgrum <notebooks.html#causal-bayesian-networks>`_.
* Some `implemented examples <https://webia.lip6.fr/~phw/aGrUM/BookOfWhy/>`_ from the `book of Why <http://bayes.cs.ucla.edu/WHY/>`_ from Judea Pearl and Dana Mackenzie.

**Reference**

.. toctree::
   :maxdepth: 3

   CausalModel
   CausalInference
   CausalAST
   CausalExceptions
   CausalNotebook
