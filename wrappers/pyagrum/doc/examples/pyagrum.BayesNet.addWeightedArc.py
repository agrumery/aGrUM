import pyagrum as gum

bn = gum.fastBN("A;B")
bn.addNoisyOR(gum.LabelizedVariable("C", "noisy OR", 2), 0.0)
# 2
bn.addWeightedArc("A", "C", 0.9)  # causal weight of A on C
bn.parents("C")
# {0}
round(float(bn.cpt("C")[{"A": 1, "B": 0}][1]), 4)
# 0.9
