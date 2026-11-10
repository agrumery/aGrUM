import pyagrum as gum

bn = gum.fastBN("U{low|high}->Y{no|yes}<-X{no|yes}")
cm = gum.CausalModel(bn)
cf = gum.Counterfactual(cm, {"Y"}, {"X"}, {"X": "yes", "Y": "yes"}, {"X": "no"})
cf.whatif()
# {'X'}
