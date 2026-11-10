import pyagrum as gum

mpdag = gum.MPDAG.meekClosure(gum.fastPDAG("A-B->C-D"))
mpdag.existsEdge(1, 0), mpdag.existsEdge(1, 2)
# (True, False)
