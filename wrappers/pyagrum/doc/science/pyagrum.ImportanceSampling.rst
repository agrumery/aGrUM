ImportanceSampling draws the samples from an importance function and weights each sample by the ratio of its
probabilities in the Bayesian network and in the importance function :cite:`shachter1990simulation`. The importance
function is the network where the observed variables are fixed to their values in the CPTs of their children; its
tiny non-zero probabilities are lifted to bound the weights, a heuristic close to the one of
:cite:`cheng2000ais` (the importance function is not adapted during the sampling). The sampling stops when the largest
95% Wilson score interval :cite:`wilson1927probable` of the estimated probabilities is small enough (or a limit of
time or iterations is reached).
The estimator, its stopping criterion and the effective size of a weighted sample are detailed in the notes of
`pyagrum.WeightedSampling`.
