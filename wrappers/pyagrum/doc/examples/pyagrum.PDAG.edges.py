import pyagrum as gum

pdag = gum.fastPDAG("A->B-C;C-D")
sorted(pdag.edges())
# [(1, 2), (2, 3)]
