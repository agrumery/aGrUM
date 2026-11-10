import pyagrum as gum

bn = gum.fastBN("A->B->C")
bn2 = bn.contextualize({"A": 1}, {"C": 0})  # observation A=1, intervention C=0
bn2.arcs()
# set()
bool((bn2.cpt("B")[:] == bn.cpt("B")[{"A": 1}]).all())  # P(B | A=1)
# True
bn2.cpt("C").tolist()
# [1.0, 0.0]
