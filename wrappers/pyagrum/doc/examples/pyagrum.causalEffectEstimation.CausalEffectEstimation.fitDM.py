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
cee.identifyAdjustmentSet("T", "Y", verbose=False)
# 'Randomized Controlled Trial'
cee.fitDM()
round(float(cee.estimateCausalEffect()), 1)
# 3.1
