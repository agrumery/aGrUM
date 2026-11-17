CausalEffectEstimation estimates, from data, the effect of a binary treatment :math:`T \in \{0, 1\}` on an outcome
:math:`Y` in the potential outcomes framework :cite:`rubin2005causal`: the average causal effect
:math:`\tau = E[Y(1) - Y(0)]` (ACE), its mean over the rows of a query (CACE) or its value for each row given as a
data frame (ICE). The causal graph is used for identification :cite:`pearl1995causal`, the estimation itself is
statistical; the module follows the lecture notes :cite:`wager2022stats,neal2020introduction`.

**Identification** (``identifyAdjustmentSet``), tried in this order: randomized controlled trial (:math:`T` and
:math:`Y` d-separated once the arc :math:`T \to Y` is removed; the covariates are the parents of :math:`Y`); back-door
set (`pyagrum.CausalModel.backDoor`); generalized front-door :cite:`fulcher2020robust` (mediators on the directed
paths from :math:`T` to :math:`Y`, covariates gathered from the back-door sets of :math:`(T, M)` and :math:`(M, Y)`);
conditional instrument :cite:`brito2002generalized`, searched among the parents of :math:`T`, its conditioning set
being the nearest separator of :cite:`van2015efficiently`. Otherwise only the estimator by do-calculus can be used.

**Estimators** (:math:`\hat e(x) = \hat P(T=1 \mid x)`, :math:`\hat\mu` regressions of the outcome; by default
linear regressions for the outcomes and logistic regressions for the probabilities):

- RCT: the difference in means :math:`\bar y_1 - \bar y_0`.
- Back-door :cite:`kunzel2019metalearners,lunceford2004stratification`: S-learner
  :math:`\hat\mu(x, 1) - \hat\mu(x, 0)` (one regression with :math:`T` as a feature); T-learner
  :math:`\hat\mu_1(x) - \hat\mu_0(x)` (one regression per group); X-learner: the imputed effects
  :math:`y - \hat\mu_0(x)` of the treated and :math:`\hat\mu_1(x) - y` of the controls are regressed into
  :math:`\hat\tau_1` and :math:`\hat\tau_0`, combined as :math:`\hat e(x)\,\hat\tau_0(x) + (1 - \hat e(x))\,\hat\tau_1(x)`
  (:math:`\hat\tau_0` relies on :math:`\hat\mu_1`, the better estimated where the treated are many);
  inverse propensity weighting :math:`\frac1n \sum_i \big(\frac{t_i}{\hat e(x_i)} - \frac{1 - t_i}{1 - \hat e(x_i)}\big) y_i`
  (not normalized); stratification into ``num_strata`` (100) strata of equal size on :math:`\hat e`, the differences
  in means of the strata being averaged with their sizes (strata without treated or control units are ignored).
- Front-door :cite:`guo2026flexible`: the plug-in estimate of
  :math:`\sum_m (P(m \mid T{=}1) - P(m \mid T{=}0)) \sum_t P(t)\,E[Y \mid m, t]`, computed as
  :math:`\frac1n \sum_i \big(\frac{\hat P(T=1 \mid m_i)}{\hat P(T=1)} - \frac{\hat P(T=0 \mid m_i)}{\hat P(T=0)}\big)
  \sum_t \hat P(t)\,\hat\mu(m_i, t)`; with covariates, :math:`\xi(m, x) = \sum_t \hat\mu(m, x, t)\,\hat P(t \mid x)` is
  regressed on :math:`x` in each group, and the ACE is the mean difference of the two regressions.
- Instrument :math:`W` (binary): the Wald estimator
  :math:`(\bar y_{W=1} - \bar y_{W=0}) / (\bar t_{W=1} - \bar t_{W=0})`, the local average effect on the compliers
  :cite:`imbens1994identification` (no covariate allowed); its inverse probability weighted versions, plain and
  normalized, with :math:`\hat P(W=1 \mid x)` :cite:`choi2021instrumental`; the two-stage least squares
  :cite:`angrist1995two` (coefficient of :math:`\hat t` in the regression of :math:`y` on :math:`x` and :math:`\hat t`,
  :math:`\hat t` being the regression of :math:`t` on :math:`x` and :math:`w`).
- Causal Bayesian network: the parameters of the network are learned from the data (smoothing prior
  :math:`10^{-9}`) and the ACE is :math:`E[Y \mid do(T{=}1)] - E[Y \mid do(T{=}0)]` (`pyagrum.causalImpact`, the labels
  of :math:`Y` read as numbers); with an instrument, the ratio of the differences of :math:`E[Y \mid w]` and
  :math:`E[T \mid w]`.
- Any estimator with the interface of CausalML :cite:`chen2020causalml`.

**Implementation limit.** The causal Bayesian network estimator does not keep the latent variables of the causal
model: its do-calculus is done as if there were none.
