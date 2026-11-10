import pyagrum as gum

gum.initRandom(42)
bn = gum.fastBN("A->B->C")
before = bn.cpt("C")[:].copy()
bn.generateCPTs()  # new random parameters for all the nodes
bool((bn.cpt("C")[:] != before).any())
# True
bn.check()
# ()
