Learning
--------

pyAgrum encloses all the learning processes for Bayesian network in a simple class BNLearner. This class gives access directly to the complete learning algorithm and theirs parameters (such as prior, scores, constraints, etc.) but also proposes low-level functions that eases the work on developping new learning algorithms (for instance, compute chi2 or conditioanl likelihood on the database, etc.).

.. autoclass:: pyAgrum.BNLearner
			:exclude-members: asIApproximationSchemeConfiguration, thisown
