import pyagrum as gum

bn = gum.fastBN("X->Y->Z")
bn.cpt("X").fillWith([0.4, 0.6])
bn.cpt("Y").fillWith([0.9, 0.1, 0.2, 0.8])
bn.cpt("Z").fillWith([0.7, 0.3, 0.1, 0.9])
cm = gum.CausalModel(bn)
formula, impact, explanation = gum.causalImpact(cm, on="Z", doing="X")
explanation
# 'backdoor [] found.'
formula.toLatex()
# 'P\\left(Z \\mid \\text{do}(X)\\right) = P\\left(Z\\mid X\\right)'
print(impact)
#
#       ║  Z                │
# X     ║0        │1        │
# ──────║─────────│─────────│
# 0     ║ 0.6400  │ 0.3600  │
# 1     ║ 0.2200  │ 0.7800  │
