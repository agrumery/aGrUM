# requires: pandas, sklearn
import numpy as np
import pandas as pd

import pyagrum as gum
import pyagrum.causalEffectEstimation as pycee

rng = np.random.default_rng(42)
X = rng.normal(size=500)
T = (rng.random(500) < 1 / (1 + np.exp(-X))).astype(int)
Y = 2 * T + X + rng.normal(size=500)  # the causal effect of T on Y is 2
df = pd.DataFrame({"X": X, "T": T, "Y": Y})
cee = pycee.CausalEffectEstimation(df, gum.CausalModel(gum.fastBN("X->T->Y;X->Y")))
cee.identifyAdjustmentSet("T", "Y", verbose=False)
# 'Backdoor'
cee.fitSLearner()
round(float(cee.estimateCausalEffect()), 1)
# 2.1
