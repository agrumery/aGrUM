WeightedSampling is likelihood weighting :cite:`fung1990weighing,shachter1990simulation`: the observed variables are
set to their observed values, the other ones are drawn forward from their CPT, and each sample is weighted by the
probability of the evidence given its parents. The sampling stops when the largest 95% Wilson score interval
:cite:`wilson1927probable` of the estimated probabilities is small enough (or a limit of time or iterations is
reached).
