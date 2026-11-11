BNClassifier is a classifier whose model is a Bayesian network learned with :class:`pyagrum.BNLearner` (MIIC
:cite:`verny2017learning`, greedy hill climbing, tabu search :cite:`glover1989tabu`) or with a fixed-form structure:
naive Bayes, the Chow-Liu tree :cite:`chow1968approximating`, or the tree-augmented naive Bayes (TAN)
:cite:`friedman1997bayesian` (here, the edges between features whose conditional mutual information given the class is
below the average are dropped, so the tree may be a forest).
