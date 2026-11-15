CausalImpact identifies the distribution :math:`P(Y \mid do(X), Z)` in a causal model (`pyagrum.CausalModel`), that
is, writes it as a formula of the observational distribution :math:`P`, then evaluates the formula, each of its terms
being computed by exact inference in the observational Bayesian network. The methods are tried in this order.

1. For a single cause, a single effect and no :math:`Z`: the back-door adjustment with the set :math:`W` given by
   `pyagrum.CausalModel.backDoor` (see `pyagrum.DoorCriteria` for the criteria and the choice of the set), then the
   front-door adjustment with a non-empty set of mediators :math:`M` :cite:`pearl1995causal`:

   .. math::

      P(y \mid do(x)) = \sum_w P(y \mid x, w)\,P(w),
      \qquad
      P(y \mid do(x)) = \sum_m P(m \mid x) \sum_{x'} P(y \mid x', m)\,P(x').

2. If :math:`Y` and :math:`X` are d-separated by :math:`Z` in the causal DAG without the arcs into :math:`X(Z)`,
   the variables of :math:`X` which are not ancestors of :math:`Z` (third rule of the do-calculus), there is no causal
   effect and the answer is :math:`P(Y \mid Z)`.
3. Otherwise, the general algorithms of Shpitser and Pearl. ID :cite:`shpitser2006identificationjoint` handles
   :math:`P(Y \mid do(X))`: it restricts the model to the ancestors of :math:`Y`, adds to :math:`X` the variables
   without effect on :math:`Y`, and factorizes the query along the c-components :cite:`tian2002general` (two
   observed variables are in the same c-component when they are linked by a chain of shared latent parents). IDC
   :cite:`shpitser2006identificationconditional` handles :math:`Z \neq \emptyset`: a variable :math:`z \in Z` such
   that :math:`Y` and :math:`z` are d-separated by :math:`X \cup Z \setminus \{z\}` in the graph without the arcs
   into :math:`X` and out of :math:`z` is moved into the intervention (second rule of the do-calculus), then

   .. math::

      P(y \mid do(x), z) = \frac{P(y, z \mid do(x))}{P(z \mid do(x))},

   both terms being computed by ID.

ID and IDC are complete: when they fail, the effect is not identifiable from :math:`P` in this causal DAG
:cite:`shpitser2006identificationjoint,shpitser2006identificationconditional`. ID then finds a hedge, a pair of
nested c-forests which witnesses the non-identifiability. The CausalImpact is not identified (``isIdentified()`` is false
and ``eval()`` raises an exception), `pyagrum.causalImpact` returns no tensor, and the explanation names the
c-components of the hedge. The formula is returned as built by the method that succeeded, without simplification.
