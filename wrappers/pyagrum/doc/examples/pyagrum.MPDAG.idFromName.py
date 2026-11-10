import pyagrum as gum

mpdag = gum.MPDAG.meekClosure(gum.fastPDAG("A-B->C-D"))
mpdag.idFromName("C")
# 2
mpdag.idFromName("Z") is None
# True
