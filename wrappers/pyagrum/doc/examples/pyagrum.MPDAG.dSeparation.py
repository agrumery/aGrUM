import pyagrum as gum

mpdag = gum.MPDAG.meekClosure(gum.fastPDAG("A-B->C-D"))
mpdag.dSeparation(0, 3)
# False
mpdag.dSeparation(0, 3, 2)
# True
