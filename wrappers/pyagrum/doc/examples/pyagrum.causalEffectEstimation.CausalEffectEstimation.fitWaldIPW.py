# requires: pandas
import numpy as np
import pandas as pd

import pyagrum as gum
import pyagrum.causalEffectEstimation as pycee

rng = np.random.default_rng(42)
W, U = rng.normal(size=500), rng.normal(size=500)  # U is not observed
Z = (rng.random(500) < 1 / (1 + np.exp(-W))).astype(int)  # the instrument
T = (Z + U + 0.3 * W + 0.5 * rng.normal(size=500) > 0.5).astype(int)
Y = 2 * T + U + W + rng.normal(size=500)  # the causal effect of T on Y is 2
df = pd.DataFrame({"W": W, "Z": Z, "T": T, "Y": Y})
cm = gum.CausalModel(gum.fastBN("Z->T->Y;W->Z;W->T;W->Y"), [("U", ["T", "Y"])])
cee = pycee.CausalEffectEstimation(df, cm)
cee.identifyAdjustmentSet("T", "Y", verbose=False)
# 'Generalized Instrumental Variable'
cee.fitWaldIPW()
round(float(cee.estimateCausalEffect()), 1)
# 1.9
