import pyagrum as gum

pdag = gum.fastPDAG("A->B-C;D->B")
sorted(pdag.arcs())
# [(0, 1), (3, 1)]
sorted(pdag.edges())
# [(1, 2)]
