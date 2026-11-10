import pyagrum as gum

bn = gum.fastBN("A->B->C")
bn.eraseArc("A", "B")
bn.arcs()
# {(1, 2)}
bn.cpt("B").names
# ('B',)
