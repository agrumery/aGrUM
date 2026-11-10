import pyagrum as gum

bn = gum.fastBN("A->B")
bn.cpt("A").fillWith([0.3, 0.7])
bn.cpt("B").fillWith([0.9, 0.1, 0.2, 0.8])
[round(p, 4) for p in gum.getPosterior(bn, target="A", evs={"B": "1"}).tolist()]
# [0.0508, 0.9492]
