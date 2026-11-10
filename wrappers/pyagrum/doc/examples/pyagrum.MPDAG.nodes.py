import pyagrum as gum

mpdag = gum.MPDAG.meekClosure(gum.fastPDAG("A-B->C-D"))
mpdag.nodes()
# {0, 1, 2, 3}
