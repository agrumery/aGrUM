import pyagrum as gum

pdag = gum.fastPDAG("A->B->C-D")
pdag.descendants(0)  # only the arcs are followed
# {1, 2}
