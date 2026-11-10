# requires: pandas, sklearn
import numpy as np
import pandas as pd

import pyagrum as gum
import pyagrum.causalEffectEstimation as pycee

rng = np.random.default_rng(42)
U = rng.normal(size=500)  # U is not observed
Z = rng.integers(0, 2, size=500)  # the instrument
T = (Z + U + 0.5 * rng.normal(size=500) > 0.5).astype(int)
Y = 2 * T + U + rng.normal(size=500)  # the causal effect of T on Y is 2
df = pd.DataFrame({"Z": Z, "T": T, "Y": Y})
cee = pycee.CausalEffectEstimation(df, gum.CausalModel(gum.fastBN("Z->T->Y"), [("U", ["T", "Y"])]))
cee.identifyAdjustmentSet("T", "Y", verbose=False)
# 'Generalized Instrumental Variable'
cee.fitWald()
round(float(cee.estimateCausalEffect()), 1)
# 2.3
