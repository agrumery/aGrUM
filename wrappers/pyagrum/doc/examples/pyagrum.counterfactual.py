import pyagrum as gum

bn = gum.fastBN("X->Y->Z")
bn.cpt("X").fillWith([0.4, 0.6])
bn.cpt("Y").fillWith([0.9, 0.1, 0.2, 0.8])
bn.cpt("Z").fillWith([0.7, 0.3, 0.1, 0.9])
cm = gum.CausalModel(bn)
# Y=1 was observed: what would Z have been if X had been 0 ?
print(gum.counterfactual(cm, on="Z", whatif="X", profile={"Y": "1"}, values={"X": "0"}))
#
#   Z                │
# 0        │1        │
# ─────────│─────────│
#  0.6400  │ 0.3600  │
