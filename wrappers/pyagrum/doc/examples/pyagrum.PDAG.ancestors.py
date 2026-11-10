import pyagrum as gum

pdag = gum.fastPDAG("A->B->C-D")
pdag.ancestors(2)
# {0, 1}
pdag.ancestors(3)  # only the arcs are followed
# set()
