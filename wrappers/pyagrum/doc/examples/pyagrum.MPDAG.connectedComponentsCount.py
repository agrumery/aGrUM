import pyagrum as gum

mpdag = gum.MPDAG.meekClosure(gum.fastPDAG("A-B->C;D-E"))
mpdag.connectedComponentsCount()
# 2
