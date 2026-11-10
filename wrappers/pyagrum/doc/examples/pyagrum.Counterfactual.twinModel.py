import pyagrum as gum

bn = gum.fastBN("U{low|high}->Y{no|yes}<-X{no|yes}")
bn.cpt("U").fillWith([0.5, 0.5])
bn.cpt("Y").fillWith([1, 0, 0.6, 0.4, 0.9, 0.1, 0.1, 0.9])
cm = gum.CausalModel(bn)
cf = gum.Counterfactual(cm, {"Y"}, {"X"}, {"X": "yes", "Y": "yes"}, {"X": "no"})
# the prior of U (the unobserved cause of Y) is updated by the profile
cf.twinModel().observationalBN().cpt("U").tolist()
# [0.1, 0.9]
