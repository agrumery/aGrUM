Inference : Variable Elimination
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Exact Inference in Continuous Linear Gaussian)
----------------------------------------------

Exact inference in the Continuous Linear Gaussian (CLG) model is performed using the Variable Elimination algorithm. The algorithm is implemented in the Inference class.
Canonical form transformations are used to perform exact inference in the Continuous Linear Gaussian (CLG) model. A junction tree generator is employed to determine the variable elimination order for the sum-product algorithm.

The inference begins with construction of canonical forms from the CLG parameters. These canonical forms are then subjected to the sum-product variable elimination algorithm to compute posterior densities.

The Inference class allows for the handling of evidence - observation data that can influence the probabilities in the model. These are integrated into the inference calculations.

The class provides functionality to update evidence with new observations, check if evidence exists for a particular variable, and erase one or all pieces of evidence from the model.

The core functionality of the Inference class is to calculate the posterior density of specified variables given current evidence in the CLG model. The posterior density is returned as a canonical form.

.. autoclass:: pyagrum.clg.CLGVariableElimination


Tool for approximated inference in Continuous Linear Gaussian)
--------------------------------------------------------------
The Forward Sampling algorithm is a tool for approximated inference in Continuous Linear Gaussian (CLG) models. It allows for sampling from the joint distribution of the model, which can be useful for generating synthetic data or for approximating posterior distributions when exact inference is computationally expensive.

.. autoclass:: pyagrum.clg.ForwardSampling