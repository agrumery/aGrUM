A credal network replaces each conditional probability table of a Bayesian network by a credal set (a convex set of
conditional distributions, given by its vertices or by probability intervals) and represents the strong extension of
these local credal sets :cite:`cozman2000credal`. The local credal sets can be learned from counts with the imprecise
Dirichlet model (``idmLearning``) :cite:`walley1996inferences`. ``approximatedBinarization`` replaces each variable by a
cluster of binary variables, as in the binarization of :cite:`antonucci2010generalized`, keeping for each bit only the
lower and upper probabilities: an outer approximation of the original credal sets, used by the L2U algorithm.
