Causality
=========

.. figure:: _static/causal.png
    :align: center
    :alt: Causality in pyAgrum

Causality in pyAgrum mainly consists in the ability to build a causal model, i.e. a (observational) Bayesian network and a set of latent variables and their relation with observation variables and in the abilidy to compute using do-calculus the causal impact in such a model.

Causality is a set of pure python3 scripts based on pyAgrum's tools.

.. note::
    In the figure above, `pyAgrum.causal` module can use a LaTeX special arrow (:math:`\hookrightarrow`) to compactly represent an intervention. By default, it uses the classical "do" notation. You can change this behavior to anything you want by using the following configuration keys:

    .. code::

            pyAgrum.config["causal","latex_do_prefix"]="\hookrightarrow("
            pyAgrum.config["causal","latex_do_suffix"]=")"

Causal models and inference
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. toctree::
   :maxdepth: 2

   CausalModel
   CausalInference

Tutorials on causality
^^^^^^^^^^^^^^^^^^^^^^

* `Notebooks on causality in pyAgrum <notebooks.html#causal-bayesian-networks>`_.
* Some `implemented examples <https://webia.lip6.fr/~phw/aGrUM/BookOfWhy/>`_ from the `book of Why <http://bayes.cs.ucla.edu/WHY/>`_ from Judea Pearl and Dana Mackenzie.


Tools for causality
^^^^^^^^^^^^^^^^^^^

.. toctree::
   :maxdepth: 2

   CausalExceptions
   CausalNotebook
   CausalAST
