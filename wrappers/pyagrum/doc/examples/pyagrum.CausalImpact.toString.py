import pyagrum as gum

bn = gum.fastBN("Z->X->Y;Z->Y")
cm = gum.CausalModel(bn)
ci = gum.CausalImpact(cm, on="Y", doing="X")
ci.toString()
# 'sum on Z for\n| *\n| | P(Y|X,Z)\n| | joint P(Z)'
