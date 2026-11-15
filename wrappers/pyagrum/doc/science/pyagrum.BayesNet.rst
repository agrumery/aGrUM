A Bayesian network represents a joint distribution by a DAG and the conditional probability table (CPT) of each
variable given its parents :cite:`pearl1988probabilistic`:
:math:`P(x_1, \dots, x_n) = \prod_i P(x_i \mid \mathrm{pa}(x_i))`.

**Arc reversal.** ``reverseArc(X, Y)`` keeps the joint distribution, as in :cite:`shachter1986evaluating`: both
nodes receive the union of their parents (:math:`X` keeping :math:`Y` as a new parent), and the new CPTs are

.. math::

   P(y \mid \mathrm{pa}) = \sum_x P(x \mid \mathrm{pa}(x))\, P(y \mid \mathrm{pa}(y)), \qquad
   P(x \mid y, \mathrm{pa}) = \frac{P(x \mid \mathrm{pa}(x))\, P(y \mid \mathrm{pa}(y))}{P(y \mid \mathrm{pa})}.

The reversal is refused when another directed path from :math:`X` to :math:`Y` would make a cycle.

**Canonical tables.** The noisy-OR and noisy-AND are canonical models with independence of causal influence
:cite:`diez2006canonical`, for a binary variable :math:`C` with binary parents :math:`X_k`, a leak :math:`\ell`
(``external_weight``) and a weight :math:`p_k` for each parent. With compound parameters (``addNoisyOR``, alias of
``addNoisyORCompound``), :math:`p_k` is the probability of :math:`C=1` when only :math:`X_k` is present, leak
included, as in the leaky noisy-OR of :cite:`henrion1987practical`; with net parameters (``addNoisyORNet``),
:math:`p_k` excludes the leak:

.. math::

   P(C = 0 \mid x) = (1-\ell) \prod_{k : x_k = 1} \frac{1-p_k}{1-\ell} \quad \text{(compound)}, \qquad
   P(C = 0 \mid x) = (1-\ell) \prod_{k : x_k = 1} (1-p_k) \quad \text{(net)}.

pyAgrum's noisy-AND is :math:`P(C = 1 \mid x) = \ell \prod_{k : x_k = 1} p_k \prod_{k : x_k = 0} (1-p_k)`, and the
logit table is the logistic regression :math:`P(C=1 \mid x) = 1/(1 + e^{-(\ell + \sum_k p_k v_k)})`, :math:`v_k`
being the numerical value of :math:`x_k`. The aggregators (``addOR``, ``addMAX``, ``addCOUNT``...) are deterministic
tables: a function of the values of the parents.
