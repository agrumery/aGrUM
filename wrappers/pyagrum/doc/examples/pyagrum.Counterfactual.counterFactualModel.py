import pyagrum as gum

bn = gum.fastBN("U{low|high}->Y{no|yes}<-X{no|yes}")
cm = gum.CausalModel(bn)
twin = gum.Counterfactual.counterFactualModel(cm, {"X"})
# Y* has the CPT of Y, with X* (no parent, uniform prior) instead of X
sorted(twin.observationalBN().names())
# ['U', 'X', 'X*', 'Y', 'Y*']
