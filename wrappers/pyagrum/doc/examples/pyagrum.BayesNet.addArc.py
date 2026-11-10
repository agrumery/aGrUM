import pyagrum as gum

bn = gum.fastBN("A;B;C")
bn.addArc("A", "B")
bn.addArc(1, 2)  # with the ids of B and C
bn.arcs()
# {(0, 1), (1, 2)}
bn.cpt("B").names  # the CPT of the head now depends on the tail
# ('B', 'A')
