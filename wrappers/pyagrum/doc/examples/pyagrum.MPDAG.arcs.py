import pyagrum as gum

mpdag = gum.MPDAG.meekClosure(gum.fastPDAG("A-B->C-D"))
sorted(mpdag.arcs())  # C->D is oriented by the Meek rules
# [(1, 2), (2, 3)]
