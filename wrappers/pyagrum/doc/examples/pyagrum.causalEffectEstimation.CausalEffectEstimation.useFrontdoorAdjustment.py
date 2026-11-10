# requires: pandas, sklearn
import numpy as np
import pandas as pd

import pyagrum as gum
import pyagrum.causalEffectEstimation as pycee

rng = np.random.default_rng(42)
W, U = rng.normal(size=500), rng.normal(size=500)  # U is not observed
T = (rng.random(500) < 1 / (1 + np.exp(-U - W))).astype(int)
M = T + 0.5 * rng.normal(size=500)
Y = 2 * M + U + W + rng.normal(size=500)  # the causal effect of T on Y is 2
df = pd.DataFrame({"W": W, "T": T, "M": M, "Y": Y})
cm = gum.CausalModel(gum.fastBN("T->M->Y;W->T;W->M;W->Y"), [("U", ["T", "Y"])])
cee = pycee.CausalEffectEstimation(df, cm)
cee.useFrontdoorAdjustment("T", "Y", {"M"}, {"W"})  # no check of the causal graph
cee.fitGeneralizedPlugIn()
round(float(cee.estimateCausalEffect()), 1)
# 1.9
