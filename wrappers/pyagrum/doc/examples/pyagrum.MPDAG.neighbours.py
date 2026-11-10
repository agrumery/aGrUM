import pyagrum as gum

mpdag = gum.MPDAG.meekClosure(gum.fastPDAG("A-B->C-D"))
mpdag.neighbours(1)  # the arc B->C is not counted
# {0}
