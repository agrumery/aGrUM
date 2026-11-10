import pyagrum as gum

bn = gum.fastBN("A;B")
bn.addNoisyAND(gum.LabelizedVariable("C", "noisy AND", 2), 1.0)
# 2
bn.addWeightedArc("A", "C", 0.9)
bn.addWeightedArc("B", "C", 0.8)
round(float(bn.cpt("C")[{"A": 1, "B": 1}][1]), 4)  # 0.9 * 0.8
# 0.72
