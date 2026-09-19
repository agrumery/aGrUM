Causal Model in pyAgrum
=======================

.. figure:: _static/causal.png
    :align: center
    :alt: Causality in pyAgrum

Causality in pyAgrum primarily involves building a causal model—that is, constructing an (observational) Bayesian network along with a set of latent variables and defining their relationships with observed variables. It also includes the ability to compute causal effects in such models using do-calculus.

pyAgrum provides a set of tools to perform causal inference and estimate causal effects from data. This includes the ability to identify interventions, compute causal impacts, and evaluate the effects of interventions on observed variables.

.. note::
    The causal module can use a LaTeX special arrow (:math:`\hookrightarrow`) to compactly represent an intervention. By default, it uses the classical "do" notation. You can change this behavior using the following configuration keys:

    .. code-block:: python

            pyagrum.config["causal","latex_do_prefix"]="\hookrightarrow("
            pyagrum.config["causal","latex_do_suffix"]=")"

A :class:`pyagrum.CausalModel` pairs a causal DAG with latent (hidden) variables and explicit
causal assumptions. It is the entry point for do-calculus reasoning and causal effect
identification.

Most of the time, a :class:`pyagrum.CausalModel` extends a :class:`pyagrum.BayesNet`: the model
then carries the observational CPTs needed to actually evaluate causal effects. It can also be
built from a plain named :class:`pyagrum.DAG` instead, with no BayesNet at all -- every structural
operation (backdoor/frontdoor, d-separation, ``toDot``, induced sub-models, do-calculus
identification) still works, but anything that reads a CPT raises
:class:`pyagrum.OperationNotAllowed`. Check :func:`pyagrum.CausalModel.hasObservationalBN` to know
which case you are in.

.. seealso::

   :doc:`CausalInference`
      Functions for computing causal impacts and applying do-calculus.

.. autoclass:: pyagrum.CausalModel

Backdoor and frontdoor criteria
--------------------------------

.. autoclass:: pyagrum.DoorCriteria
