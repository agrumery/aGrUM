import pyagrum as gum

mr = gum.MeekRules()
g = mr.propagate(gum.fastMixedGraph("A->B-C"))  # rule R1 orients B-C into B->C
sorted(g.arcs()), g.edges()
# ([(0, 1), (1, 2)], set())
