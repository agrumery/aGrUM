import pyagrum as gum

pdag = gum.fastPDAG("A->B-C;C->D")
pdag.sizeArcs()
# 2
