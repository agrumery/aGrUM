CLGLearner learns the structure of a Gaussian Bayesian network with the order-independent PC algorithm of
:cite:`colombo2014order`, whose independence tests on partial correlations bound the family-wise error rate with
Rademacher averages :cite:`simionato2022bounding`. The v-structures and the orientation rules of
:cite:`meek1995causal` are followed by a pyAgrum heuristic that orients the remaining edges by increasing variance.
The parameters are then estimated by linear regression.
