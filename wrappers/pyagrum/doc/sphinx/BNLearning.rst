Learning
=======
pyAgrum provides a complete framework for learning Bayesian networks from data. It includes various algorithms for structure learning, parameter learning, and model evaluation. The library supports both score-based and constraint-based approaches, allowing users to choose the method that best fits their needs.

pyAgrum brings together all Bayesian network learning processes in a single, easy-to-use class: :class:`pyagrum.BNLearner`. This class provides direct access to complete learning algorithms and their parameters (such as priors, scores, constraints, etc.), and also offers low-level functions that facilitate the development of new learning algorithms (for example, computing chi² or conditional likelihood on the dataset).

BNLearner allows to choose :

- the structure learning algorithm (MIIC, Greedy Hill Climbing, K2, etc.),
- the parameter learning method (including EM),
- the scoring function (BDeu, AIC, etc.) for score-based algorithms,
- the prior (smoothing, Dirichlet, etc.),
- the constraints (e.g., forbidding certain arcs, specifying a partial order among variables, etc.),
- the correction method (NML, etc.) for the MIIC algorithm,
- and many low-level functions, such as computing the chi², G² score, or the conditional likelihood on the dataset.

:class:`pyagrum.BNLearner` is able to learn a Bayesian network from a database (a pandas.DataFrame) or from a csv file.

Class BNLearner
--------------- 
.. autoclass:: pyagrum.BNLearner
  :exclude-members: asIApproximationSchemeConfiguration, thisown
