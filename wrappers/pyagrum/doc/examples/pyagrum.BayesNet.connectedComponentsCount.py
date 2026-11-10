import pyagrum as gum

bn = gum.fastBN("A->B;C->D;E")
bn.connectedComponentsCount()
# 3
