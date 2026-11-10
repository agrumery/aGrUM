# requires: pandas, sklearn
import numpy as np
import pandas as pd

import pyagrum as gum
import pyagrum.causalEffectEstimation as pycee

rng = np.random.default_rng(42)
T = rng.integers(0, 2, size=500)  # randomized treatment
Y = 3 * T + rng.normal(size=500)  # the causal effect of T on Y is 3
df = pd.DataFrame({"T": T, "Y": Y})
cee = pycee.CausalEffectEstimation(df, gum.CausalModel(gum.fastBN("T->Y")))


class DiffInMeans:  # an estimator with the methods fit, predict and estimate_ate
  def fit(self, X, treatment, y):
    self.ace = y[treatment == 1].mean() - y[treatment == 0].mean()

  def predict(self, X, treatment, y):
    return np.full(len(X), self.ace)

  def estimate_ate(self, X, treatment, y):
    return self.ace


cee.identifyAdjustmentSet("T", "Y", verbose=False)
# 'Randomized Controlled Trial'
cee.fitCustomEstimator(DiffInMeans())
round(float(cee.estimateCausalEffect()), 1)
# 3.1
