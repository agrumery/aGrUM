import pyagrum as gum

pdag = gum.fastPDAG("A->B-C;B-D")
pdag.neighbours(1)  # the arc A->B is not counted
# {2, 3}
