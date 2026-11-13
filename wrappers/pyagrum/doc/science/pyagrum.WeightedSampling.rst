WeightedSampling is likelihood weighting :cite:`fung1990weighing,shachter1990simulation`: the observed variables are
set to their observed values, the other ones are drawn forward from their CPT, and each sample :math:`x^{(k)}` is
weighted by the probability :math:`w_k` of the evidence given its parents.

**The estimator** (shared by all the samplers of pyAgrum). After :math:`N` samples, the posterior of a value
:math:`x` of a variable :math:`X` is the weighted frequency

.. math::

   \hat p = \frac{\sum_{k=1}^{N} w_k \, \mathbb{1}[x^{(k)}_X = x]}{\sum_{k=1}^{N} w_k}

(all the weights are 1 for ``MonteCarloSampling`` and ``GibbsSampling``, and the ratio of the probabilities in the
network and in the importance function for ``ImportanceSampling``).

**The stopping criterion.** Each :math:`\hat p` is seen as the parameter of a Bernoulli variable estimated from a
sample of size :math:`n`, and the sampling stops when the largest length of the 95% Wilson score intervals
:cite:`wilson1927probable` (:math:`z = 1.96`)

.. math::

   \ell = \frac{2z}{1 + z^2/n} \sqrt{\frac{\hat p (1 - \hat p)}{n} + \frac{z^2}{4n^2}}

is below ``epsilon`` (or when a limit of time, of iterations or of the decrease rate of :math:`\ell` is reached).
Unlike the Wald interval :math:`2z\sqrt{\hat p(1-\hat p)/n}`, :math:`\ell` never vanishes: it is :math:`z^2/(n+z^2)`
when all the samples agree, so that a few identical samples are not taken for a convergence.

**The size of a weighted sample.** :math:`N` weighted samples do not carry the information of :math:`N` samples of
the posterior: when a few large weights dominate, the estimate rests on a few samples. The effective sample size of
importance sampling, :math:`N / (1 + \mathrm{Var}(W))` for weights :math:`W` normalized to a mean of 1, comes from the
work of Kong and is used in :cite:`kong1994sequential`; the size :math:`n` is its usual estimate from the sample
:cite:`elvira2022rethinking`

.. math::

   n_{\text{eff}} = \frac{\left(\sum_{k=1}^{N} w_k\right)^2}{\sum_{k=1}^{N} w_k^2}
                  = \frac{1}{\sum_{k=1}^{N} \bar w_k^2}, \qquad \bar w_k = \frac{w_k}{\sum_j w_j},

which is :math:`N` when the weights are equal (whatever their value) and decreases down to 1 when a single weight
dominates. It is an approximation, which ignores the function being estimated :cite:`elvira2022rethinking`: the
criterion is a heuristic, not a guarantee on the error of each posterior.

**With a prior** (``LoopyWeightedSampling``, ``LoopyImportanceSampling``...): the posteriors :math:`\pi` given by a
loopy belief propagation count as :math:`n_v` virtual samples. The estimate becomes
:math:`(n_v\,\pi + N\,\hat p)/(n_v + N)`, the size :math:`n = n_v + n_{\text{eff}}`, and the criteria on the precision
are only checked once the real samples outnumber the virtual ones (:math:`N \geq n_v`).
