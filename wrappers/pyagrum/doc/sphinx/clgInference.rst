Inference : Variable Elimination
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Exact inference in Conditional Linear Gaussian
----------------------------------------------

Exact inference in a Conditional Linear Gaussian (CLG) model is performed by variable elimination, implemented in
the class `pyagrum.clg.CLGVariableElimination`. The densities of the model are written as canonical forms, and a
junction tree generator gives the elimination order; the elimination multiplies canonical forms and integrates the
eliminated variables out.

Evidence (observed values of variables) can be added, updated, tested and erased; it is integrated into the canonical
forms before the elimination.

The posterior density of one or several variables given the current evidence is returned as a canonical form
(``canonicalPosterior``), or as a `pyagrum.clg.GaussianVariable` for one variable (``posterior``).

.. pyagrum-class:: pyagrum.clg.CLGVariableElimination


Forward sampling
----------------
Forward sampling draws samples from the joint distribution of a CLG model, without evidence: each variable is drawn
given its parents, in a topological order. The samples can be used to generate synthetic data, or to estimate means,
variances and covariances of the variables.

.. pyagrum-class:: pyagrum.clg.ForwardSampling
