import pyagrum as gum

bn = gum.fastBN("Z->X->Y;Z->Y")
cm = gum.CausalModel(bn)
ci = gum.CausalImpact(cm, on="Y", doing="X")
ci.toLatex()
# 'P\\left(Y \\mid \\text{do}(X)\\right) = \\sum_{Z}{P\\left(Y\\mid X,Z\\right) \\cdot P\\left(Z\\right)}'
