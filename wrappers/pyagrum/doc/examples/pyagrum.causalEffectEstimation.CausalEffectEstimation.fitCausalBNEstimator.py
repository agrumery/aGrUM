# requires: pandas, sklearn
import pyagrum as gum
import pyagrum.causalEffectEstimation as pycee

gum.initRandom(42)
bn = gum.fastBN("X->T->Y;X->Y")  # binary variables
df = gum.generateSample(bn, 1000)[0].astype(int)
cee = pycee.CausalEffectEstimation(df, gum.CausalModel(bn))
cee.identifyAdjustmentSet("T", "Y", verbose=False)
# 'Backdoor'
cee.fitCausalBNEstimator()
round(float(cee.estimateCausalEffect()), 2)  # P(Y=1|do(T=1)) - P(Y=1|do(T=0))
# -0.09
