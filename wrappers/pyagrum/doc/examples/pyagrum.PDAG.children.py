import pyagrum as gum

pdag = gum.fastPDAG("A->B-C;A->D")
pdag.children(0)
# {1, 3}
