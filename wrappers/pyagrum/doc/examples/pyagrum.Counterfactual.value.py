import pyagrum as gum

bn = gum.fastBN("U{low|high}->Y{no|yes}<-X{no|yes}")
bn.cpt("U").fillWith([0.5, 0.5])
bn.cpt("Y").fillWith([1, 0, 0.6, 0.4, 0.9, 0.1, 0.1, 0.9])
cm = gum.CausalModel(bn)
# X=yes and Y=yes were observed: what would Y have been if X had been 'no' ?
cf = gum.Counterfactual(cm, {"Y"}, {"X"}, {"X": "yes", "Y": "yes"}, {"X": "no"})
print(cf.value())
#
#   Y                │
# no       │yes      │
# ─────────│─────────│
#  0.6400  │ 0.3600  │
