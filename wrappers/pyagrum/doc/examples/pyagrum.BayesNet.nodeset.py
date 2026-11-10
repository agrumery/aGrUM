import pyagrum as gum

bn = gum.fastBN("A->B->C")
bn.nodeset(["C", "A"])
# {0, 2}
