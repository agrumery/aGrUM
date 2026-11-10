import pyagrum as gum

mpdag = gum.MPDAG.meekClosure(gum.fastPDAG("A-B->C-D"))
mpdag.existsArc(2, 3), mpdag.existsArc(0, 1)
# (True, False)
