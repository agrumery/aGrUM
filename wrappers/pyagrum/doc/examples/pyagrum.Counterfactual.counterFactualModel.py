import pyagrum as gum

bn = gum.fastBN("U{low|high}->Y{no|yes}<-X{no|yes}")
bn.cpt("U").fillWith([0.5, 0.5])
bn.cpt("Y").fillWith([1, 0, 0.6, 0.4, 0.9, 0.1, 0.1, 0.9])
cm = gum.CausalModel(bn)
twin = gum.Counterfactual.counterFactualModel(cm, {"X": "yes", "Y": "yes"}, {"X"})
# the prior of U is replaced by its posterior given the profile
print(twin.observationalBN().cpt("U"))
#
#   U                │
# low      │high     │
# ─────────│─────────│
#  0.1000  │ 0.9000  │
