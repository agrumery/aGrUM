BNLearner learns a Bayesian network from a complete database of discrete variables, by a score-based search, by
conditional independence tests, or by MIIC (the default algorithm, given by
``pyagrum.config["learning", "BNLearnerPreferredAlgo"]``); the parameters are then estimated from the counts.

**Scores** (``useScoreXXX``, BDeu by default). They are decomposable, in bits (base 2 logarithms). With :math:`N_{ijk}`
the count of :math:`X_i = k` and of the :math:`j`-th configuration of its :math:`q_i` parent configurations,
:math:`r_i` the domain size of :math:`X_i` and :math:`N` the size of the database:

.. math::

   \mathrm{LL}_i = \sum_{j,k} N_{ijk} \log_2 \frac{N_{ijk}}{N_{ij}}, \qquad
   \mathrm{AIC}_i = \mathrm{LL}_i - q_i(r_i-1), \qquad
   \mathrm{BIC}_i = \mathrm{LL}_i - \tfrac{1}{2}\, q_i(r_i-1) \log_2 N,

.. math::

   \mathrm{BD}_i = \sum_{j=1}^{q_i} \Big[\log_2 \frac{\Gamma(N'_{ij})}{\Gamma(N_{ij}+N'_{ij})}
                 + \sum_{k=1}^{r_i} \log_2 \frac{\Gamma(N_{ijk}+N'_{ijk})}{\Gamma(N'_{ijk})}\Big]

(AIC :cite:`akaike1974new`, BIC :cite:`schwarz1978estimating`, BD :cite:`heckerman1995learning`). The penalty of
AIC being subtracted from a log-likelihood in bits, it weighs :math:`\ln 2 \approx 0.69` times Akaike's one. BD
takes its hyperparameters :math:`N'_{ijk}` from the prior, which is then mandatory; BDeu :cite:`buntine1991theory`
uses :math:`N'_{ijk} = 1/(r_i q_i)` (an equivalent sample size of 1) and K2 :cite:`cooper1992bayesian`
:math:`N'_{ijk} = 1`. A prior given to another score adds its pseudo-counts to the counts (to these implicit
hyperparameters for BDeu and K2, with a warning).

**Priors** (``useXXXPrior``, none by default, weight :math:`w` given by ``pyagrum.config["learning", "priorWeight"]``,
1): pseudo-counts :math:`N'_{ijk} = w` for the smoothing (Laplace for :math:`w=1`); :math:`w/(r_i q_i)` for BDeu;
:math:`w` times the joint distribution of the family for a Dirichlet prior from a Bayesian network, or its frequency
in another database.

**Score-based search.** Greedy hill climbing applies the best arc addition, removal or reversal until no change
improves the score; the local search with tabu list :cite:`glover1989tabu` accepts up to ``tabuNbDecrease`` (2)
consecutive decreases with a tabu list of ``tabuListSize`` (100) changes and returns the best graph found; greedy
thick-thinning adds arcs while the score improves, then removes them; K2 :cite:`cooper1992bayesian` is the greedy
search restricted to the arc additions consistent with a given order, with the chosen score.

**Independence tests** (PC, FCI). The chi2 or G2 statistic of :math:`X \perp Y \mid Z` is computed on the non empty
part of the table: a configuration :math:`z` with :math:`r_z` non empty rows and :math:`c_z` non empty columns brings
:math:`(r_z-1)(c_z-1)` degrees of freedom :cite:`agresti2013categorical`, and independence is accepted when the p-value
exceeds ``alpha`` (``pyagrum.config["learning", "alpha"]``, 0.05). PC :cite:`spirtes1991algorithm,spirtes2000causation`
is stable by default :cite:`colombo2014order`; conflicting colliders become double arcs before the Meek rules
(`pyagrum.MeekRules`). FCI gives a PAG (`pyagrum.PAG`), turned into a DAG through its representative MAG.

**MIIC** :cite:`verny2017learning` removes the edge :math:`X - Y` as soon as a conditioning set :math:`U` (grown by
the best contributors) gives :math:`N\,I(X;Y \mid U) - k_{X;Y \mid U} \le 0` (information in bits). The complexity is
:math:`k = \tfrac12 (r_X-1)(r_Y-1)\, r_U \log_2 N` for the MDL correction (default) :cite:`rissanen1978modeling`, and
for the NML one the sum, over the configurations :math:`u`, of :math:`\tfrac12 [\sum_x \log_2 C(r_Y, N_{xu}) - \log_2
C(r_Y, N_u) + \sum_y \log_2 C(r_X, N_{yu}) - \log_2 C(r_X, N_u)]`, :math:`C(r, n)` being the multinomial parametric
complexity :cite:`kontkanen2003efficient,kontkanen2007linear`. The unshielded triples are then oriented by
decreasing probability of collider, computed from the 3-point information; contradictory orientations give the double
arcs of ``latentVariables``. The DAGs of the constraint-based algorithms are oriented by `pyagrum.MeekRules`.

**Parameters.** :math:`\hat\theta_{ijk} = (N_{ijk} + N'_{ijk})/(N_{ij} + N'_{ij})`, the prior's pseudo-counts
included; ``learnBN`` adds the implicit hyperparameters of the score (BDeu, K2). With missing values and ``useEM``,
EM :cite:`dempster1977maximum,lauritzen1995em` starts from the estimate on the complete rows, mixed as
:math:`(1-\nu)\,\theta + \nu\,\text{random}` with :math:`\nu` = ``pyagrum.config["learning", "emNoise"]`` (0.1); each
iteration replaces the counts by their expectations given the current network, until the relative variation of the
log-likelihood is below ``epsilon`` (or its variation, or 10 000 iterations). EM also stops after 4 consecutive
decreases of the log-likelihood; when it ends on decreases, the network that preceded them is returned.

**Implementation limit.** Structure learning needs a complete database; without prior, a parent configuration absent
from the data makes the maximum likelihood undefined and raises an error.
