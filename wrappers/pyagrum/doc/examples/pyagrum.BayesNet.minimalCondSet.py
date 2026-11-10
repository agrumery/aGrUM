import pyagrum as gum

bn = gum.fastBN("A->B->C->D;E->C")
bn.minimalCondSet(bn.idFromName("D"), bn.ids(["A", "B", "C", "E"]))  # C is enough to compute D
# {2}
bn.minimalCondSet(bn.idFromName("D"), bn.ids(["A", "B", "E"]))
# {1, 4}
