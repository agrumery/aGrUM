import pyagrum as gum

pdag = gum.fastPDAG("A->B-C;B->D")
pdag.boundary(1)  # parents, children and neighbours
# {0, 2, 3}
