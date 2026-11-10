import pyagrum as gum

bn = gum.fastBN("A->B")
bn.cpt("A").fillWith([0.2, 0.8])
bn.cpt("B").fillWith([0.9, 0.1, 0.3, 0.7])
bn.reverseArc("A", "B")
bn.arcs()
# {(1, 0)}
[round(float(p), 4) for p in bn.cpt("B")[:]]  # the joint distribution is preserved
# [0.42, 0.58]
