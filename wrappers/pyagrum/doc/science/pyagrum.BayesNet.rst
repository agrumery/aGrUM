A Bayesian network represents a joint distribution by a DAG and the conditional probability table of each variable
given its parents :cite:`pearl1988probabilistic`. ``reverseArc`` reverses an arc while keeping the joint distribution,
by the arc reversal of :cite:`shachter1986evaluating`. The noisy-OR, noisy-AND and logit tables are canonical (ICI)
models :cite:`diez2006canonical`; the noisy-OR exists with compound parameters (``addNoisyORCompound``, the leaky
noisy-OR of :cite:`henrion1987practical`) and with net parameters (``addNoisyORNet``), see :cite:`diez2006canonical`.
