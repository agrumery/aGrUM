A causal model is a causal diagram :cite:`pearl1995causal,pearl2009causality`: a DAG whose arcs are direct causal
relations, over the observed variables and some latent (unobserved) variables. In pyAgrum a latent variable is a
parentless common cause of at least two observed variables, so that the model is semi-Markovian. An intervention
:math:`do(X=x)` replaces the mechanism of :math:`X` by the constant :math:`x`; without latent variables, its effect is
given by the truncated factorization

.. math::

   P(v \mid do(x)) = \prod_{V_i \notin X} P(v_i \mid pa_i) \Big|_{X=x},

and with latent variables it has to be identified from the observational distribution (`pyagrum.CausalImpact`).

**Two graphs.** A CausalModel holds the causal DAG (observed and latent variables), used for every structural
question (d-separation, back-door and front-door sets, identification), and the observational Bayesian network
(observed variables only), whose CPTs are used to evaluate the identified formulas. They may differ: adding a latent
variable removes, in the causal DAG only, the arcs between its children, their dependence being attributed to the
latent common cause, unless ``assumeNonSpurious`` is true. In the same way, ``assumeSpurious(x, y)`` removes the arc
:math:`x \to y` from the causal DAG and ``assumeNonSpurious(x, y)`` puts it back; both require the arc to exist in the
observational network.

A model built from a DAG instead of a Bayesian network has no parameters: the structural operations are available,
but no formula can be evaluated.
