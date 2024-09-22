Causal Effect Estimation
^^^^^^^^^^^^^^^^^^^^^^^^

The Neyman-Rubin Potential Outcomes Framework is an approach for estimating 
causal effects (also known as treatment effects) in causal inference. It defines 
causality through the potential outcomes :math:`Y` of a binary intervention :math:`T`.
The causal effect, defined as the difference between these potential outcomes, 
is the core focus of this framework. :cite:t:`rubin2005causal`

However, since only one of the potential outcomes is observed—either the unit receives 
the intervention or it does not—the difference in *potential* outcomes is unobservable.
This is known as the "Fundamental Problem of Causal Inference".

Recent advancements have developed improved statistical estimators for causal effects, 
each associated with specific causal assumptions. This module integrates these 
advancements with foundational causal identification through Bayesian networks. 
:cite:t:`pearl1995causal`
It provides a pipeline for detecting suitable adjustment sets and applying the 
appropriate estimators to achieve accurate causal effect estimations.

.. autoclass:: pyAgrum.causal.CausalEffectEstimation

.. bibliography::
