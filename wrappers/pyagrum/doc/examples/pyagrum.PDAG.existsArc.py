import pyagrum as gum

pdag = gum.fastPDAG("A->B-C")
pdag.existsArc(0, 1), pdag.existsArc(1, 0), pdag.existsArc(1, 2)
# (True, False, False)
