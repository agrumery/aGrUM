CausalImpact identifies :math:`P(Y \mid do(X), Z)` in a causal model by trying in turn d-separation, the back-door and
the front-door adjustments :cite:`pearl1995causal`, then the general identification algorithms: ID for
:math:`P(Y \mid do(X))` :cite:`shpitser2006identificationjoint`, based on the decomposition of the model into
c-components :cite:`tian2002general`, and IDC for conditional queries :cite:`shpitser2006identificationconditional`.
Both are complete: a non-identifiable effect is reported with a hedge.
