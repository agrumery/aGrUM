import pyagrum as gum

bn = gum.fastBN("U{low|high}->Y{no|yes}<-X{no|yes}")
bn.cpt("U").fillWith([0.5, 0.5])
bn.cpt("X").fillWith([0.5, 0.5])
bn.cpt("Y").fillWith([1, 0, 0.6, 0.4, 0.9, 0.1, 0.1, 0.9])
cm = gum.CausalModel(bn)
cf = gum.Counterfactual(cm, {"Y"}, {"X"}, {"Y": "yes"}, {"X": "no"})
print(cf.toString())
# [Counterfactual]
#  on = {Y}
#  whatif = {X}
#  profile: Y=yes
#  values: X=no
#  value (adapted to original variables):
#
#   Y                │
# no       │yes      │
# ─────────│─────────│
#  0.6286  │ 0.3714  │
