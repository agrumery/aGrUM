import pyagrum as gum

mr = gum.MeekRules()
cpdag = mr.propagateToCPDAG(gum.fastMixedGraph("A->B<-D;B-C"))
type(cpdag).__name__, sorted(cpdag.arcs()), cpdag.edges()
# ('PDAG', [(0, 1), (1, 3), (2, 1)], set())
