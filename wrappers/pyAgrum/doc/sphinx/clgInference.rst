Inference
^^^^^^^^^^^^^

Exact Inference
---------------

Canonical form transformations are used to perform exact inference in the Continuous Linear Gaussian (CLG) model. A junction tree generator is employed to determine the variable elimination order for the sum-product algorithm.

The inference begins with construction of canonical forms from the CLG parameters. These canonical forms are then subjected to the sum-product variable elimination algorithm to compute posterior densities.

.. autoclass:: pyAgrum.clg.Inference

Evidence Methods
----------------

The Inference class allows for the handling of evidence - observation data that can influence the probabilities in the model. These are integrated into the inference calculations.

The class provides functionality to update evidence with new observations, check if evidence exists for a particular variable, and erase one or all pieces of evidence from the model.

.. automethod:: pyAgrum.clg.Inference.updateEvidence
.. automethod:: pyAgrum.clg.Inference.hasEvidence
.. automethod:: pyAgrum.clg.Inference.eraseEvidence
.. automethod:: pyAgrum.clg.Inference.eraseAllEvidence

Posterior Density Calculation
-----------------------------

The core functionality of the Inference class is to calculate the posterior density of specified variables given current evidence in the CLG model. The posterior density is returned as a canonical form.

.. automethod:: pyAgrum.clg.Inference.posterior
