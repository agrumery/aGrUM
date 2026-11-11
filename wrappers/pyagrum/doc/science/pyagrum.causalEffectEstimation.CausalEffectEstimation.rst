CausalEffectEstimation estimates causal effects in the potential outcomes framework :cite:`rubin2005causal`, after an
identification on a causal graph :cite:`pearl1995causal` (backdoor, generalized front-door
:cite:`fulcher2020robust`, generalized instrumental variable :cite:`brito2012generalized,van2015efficiently`). The
module follows the lecture notes :cite:`wager2022stats,neal2020introduction`. Its estimators are:

- randomized controlled trial: the difference in means;
- backdoor: the S-, T- and X-learners :cite:`kunzel2019metalearners`, the propensity score stratification and the
  inverse propensity weighting :cite:`lunceford2004stratification`;
- front-door: the plug-in estimators of :cite:`guo2023targeted`;
- instrumental variable: the Wald estimator, its (normalized) inverse probability weighted versions
  :cite:`choi2021instrumental`, and the two-stage least squares :cite:`angrist1995two`;
- any estimator with the interface of CausalML :cite:`chen2020causalml`, and an estimator by do-calculus on the causal
  Bayesian network.
