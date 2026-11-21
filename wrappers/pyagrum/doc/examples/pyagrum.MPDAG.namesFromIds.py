import pyagrum as gum

mpdag = gum.MPDAG.fromCPDAG(gum.CPDAG.fromDAG(gum.fastDAG("A->B->C")), [("A", "B")])
sorted(mpdag.namesFromIds(mpdag.arcs()))  # A->B imposed, then B->C by the Meek rules
# [('A', 'B'), ('B', 'C')]
