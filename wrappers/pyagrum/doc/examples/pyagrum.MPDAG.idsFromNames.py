import pyagrum as gum

mpdag = gum.MPDAG.fromCPDAG(gum.CPDAG.fromDAG(gum.fastDAG("A->B->C")), [("A", "B")])
mpdag.idsFromNames(["C", "A"])
# [2, 0]
