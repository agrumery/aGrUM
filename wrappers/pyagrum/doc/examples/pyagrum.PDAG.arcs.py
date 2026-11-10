import pyagrum as gum

pdag = gum.fastPDAG("A->B-C;C->D")
sorted(pdag.arcs())
# [(0, 1), (2, 3)]
