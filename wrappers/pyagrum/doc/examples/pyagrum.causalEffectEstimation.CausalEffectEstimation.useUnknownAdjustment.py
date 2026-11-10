# requires: pandas, sklearn
import pyagrum as gum
import pyagrum.causalEffectEstimation as pycee

gum.initRandom(42)
bn = gum.fastBN("X->T->Y;X->Y")  # binary variables
df = gum.generateSample(bn, 1000)[0].astype(int)
cee = pycee.CausalEffectEstimation(df, gum.CausalModel(bn))
cee.useUnknownAdjustment("T", "Y")  # only the causal BN estimator can be used
cee.fitCausalBNEstimator()
round(float(cee.estimateCausalEffect()), 2)
# 0.04
