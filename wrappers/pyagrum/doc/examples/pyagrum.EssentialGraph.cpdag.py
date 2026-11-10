import pyagrum as gum

eg = gum.EssentialGraph(gum.fastBN("A->B->C"))
cpdag = eg.cpdag()
type(cpdag).__name__, sorted(cpdag.edges())
# ('CPDAG', [(0, 1), (1, 2)])
