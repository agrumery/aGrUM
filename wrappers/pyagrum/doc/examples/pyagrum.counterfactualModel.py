import pyagrum as gum

bn = gum.fastBN("X->Y->Z")
bn.cpt("X").fillWith([0.4, 0.6])
bn.cpt("Y").fillWith([0.9, 0.1, 0.2, 0.8])
bn.cpt("Z").fillWith([0.7, 0.3, 0.1, 0.9])
cm = gum.CausalModel(bn)
twin = gum.counterfactualModel(cm, profile={"Y": "1"}, whatif="X")
print(twin.observationalBN().cpt("X"))  # X, a parentless node intervened on, is not updated
#
#   X                │
# 0        │1        │
# ─────────│─────────│
#  0.4000  │ 0.6000  │
