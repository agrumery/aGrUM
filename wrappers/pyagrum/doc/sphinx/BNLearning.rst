Learning
--------

pyAgrum encloses all the learning processes for Bayesian network in a simple class BNLearner. This class gives access directly to the complete learning algorithm and their parameters (such as prior, scores, constraints, etc.) but also proposes low-level functions that eases the work on developping new learning algorithms (for instance, compute chi2 or conditioanl likelihood on the database, etc.).

BNLearner allows to choose :
- the learning algorithm (MIIC, Greedy Hill Climbing, K2, etc.)
- the parameters learning algorithm (including EM),
- the score (BDeu, AIC, etc.) for score-based algorithms
- the prior (smoothing, Dirichlet, etc.)
- the constraints (for instance, forbid some arcs, propose a partial order in the variables, etc.)
- the correction (NML, etc) for MIIC algorithm
- and many low-level function such as computing the chi2, G2 score or the conditional likelihood on the database, etc.

BNLearner is able to learn a Bayesian network from a database (a pandas.DataFrame) or from a csv file.

.. autoclass:: pyagrum.BNLearner
  :exclude-members: asIApproximationSchemeConfiguration, thisown
