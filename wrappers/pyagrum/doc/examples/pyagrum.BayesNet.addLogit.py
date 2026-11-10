import pyagrum as gum

bn = gum.fastBN("A;B")
bn.addLogit(gum.LabelizedVariable("C", "logit", 2), -1.0)  # external weight (bias) -1
# 2
bn.addWeightedArc("A", "C", 2.0)
bn.addWeightedArc("B", "C", 1.0)
round(float(bn.cpt("C")[{"A": 1, "B": 0}][1]), 4)  # P(C=1 | A=1, B=0) = sigmoid(-1 + 2)
# 0.7311
