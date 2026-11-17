KTBNAdaptiveLearner also selects the order :math:`k` of the k-TBN: for every candidate :math:`k \in [k_{min}, k_{max}]`
(:math:`k_{min} = 2`, raised when a constraint names a slice :math:`s`, to :math:`s+1`), it learns a k-TBN with
`pyagrum.ktbn.KTBNLearner` and the recorded configuration, then keeps the candidate with the best *order score*. This
outer score is distinct from the score used to learn each structure, and it is computed on the whole learned k-TBN, so
that it can be compared across :math:`k`.

**The likelihood.** All the candidates are scored on the same data: every time step of every trajectory. For a
candidate :math:`k`, the instance at time :math:`t` of a temporal variable is scored with the CPT of the slice
:math:`\min(t, k-1)` of the template (initial CPTs, then the kernel), and the atemporal variables once per trajectory:

.. math::

   \log_2 L_k = \sum_{\text{trajectories}} \Big( \sum_{C} \log_2 P(c \mid \mathrm{pa}_C)
       + \sum_{t \geq 0} \sum_{V} \log_2 P_{\min(t,k-1)}(v_t \mid \mathrm{pa}_{V_t}) \Big).

**The order scores** (to maximize), with :math:`d_k` the number of free parameters of the template (all its slices and
atemporal variables: :math:`(r_X - 1)\, q_X` for a node with :math:`r_X` values and :math:`q_X` parent configurations)
and :math:`N` the total number of time steps of all the trajectories, the same for every candidate:

- BIC :cite:`schwarz1978estimating` (default): :math:`\log_2 L_k - \frac{1}{2} d_k \log_2 N`;
- AIC :cite:`akaike1974new`: :math:`\log_2 L_k - d_k \log_2 e`, i.e. Akaike's :math:`\ln L - d` converted to bits;
- fNML :cite:`silander2008factorized`: :math:`\log_2 L_k - \sum_X \sum_j \log_2 \mathcal{C}(r_X, N_{Xj})`, the
  parametric complexity of a multinomial with :math:`r_X` values over the :math:`N_{Xj}` instances of each observed
  parent configuration :math:`j` of each node of the template (as in the fNML score of `pyagrum.BNLearner`).

A larger :math:`k` adds initial slices and lags, hence parameters: the penalty trades them against the gain in
likelihood. On ties, the smallest :math:`k` is kept. ``scorePerCandidateK`` gives the scores of all the candidates.

**Implementation limit.** With ``ignoreMissingSymbols``, an instance whose family has a missing value is not scored;
a larger :math:`k` drops more instances (its families span more time steps) while :math:`N` is not changed, which
raises its likelihood and biases the choice towards larger orders. Every trajectory must have at least :math:`k_{max}`
time steps.
