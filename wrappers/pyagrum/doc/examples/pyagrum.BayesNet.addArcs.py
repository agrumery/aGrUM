import pyagrum as gum

bn = gum.fastBN("A;B;C")
bn.addArcs([("A", "B"), ("A", "C"), ("B", "C")])
sorted(bn.arcs())
# [(0, 1), (0, 2), (1, 2)]
