import pyagrum as gum

bn = gum.fastBN("A;B")
bn.addNoisyORNet(gum.LabelizedVariable("C", "noisy OR", 2), 0.0)
# 2
bn.addWeightedArc("A", "C", 0.8)
bn.addWeightedArc("B", "C", 0.6)
[round(float(p), 4) for p in bn.cpt("C")[{"A": 1, "B": 1}]]
# [0.08, 0.92]
