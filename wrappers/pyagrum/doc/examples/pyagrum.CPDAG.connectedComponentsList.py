import pyagrum as gum

cpdag = gum.CPDAG.fromDAG(gum.fastDAG("A->B;C->D"))
sorted(sorted(c) for c in cpdag.connectedComponentsList().values())
# [[0, 1], [2, 3]]
