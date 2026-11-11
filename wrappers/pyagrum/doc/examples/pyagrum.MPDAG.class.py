import pyagrum as gum

cpdag = gum.CPDAG.fromDAG(gum.fastDAG("A->B->C"))
sorted(cpdag.edges())
# [(0, 1), (1, 2)]
mpdag = gum.MPDAG.fromCPDAG(cpdag, [(0, 1)])  # A->B imposed, then B->C by the Meek rules
sorted(mpdag.arcs()), sorted(mpdag.edges())
# ([(0, 1), (1, 2)], [])
