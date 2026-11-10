import pyagrum as gum

bn = gum.fastBN("A;B")
bn.addNoisyORCompound(gum.LabelizedVariable("C", "noisy OR", 2), 0.0)
# 2
bn.addWeightedArc("A", "C", 0.8)
bn.addWeightedArc("B", "C", 0.6)
round(float(bn.cpt("C")[{"A": 1, "B": 1}][1]), 4)  # 1 - (1 - 0.8) * (1 - 0.6)
# 0.92
