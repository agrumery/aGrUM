import pyagrum as gum

mpdag = gum.MPDAG.meekClosure(gum.fastPDAG("A-B->C-D"))
mpdag.edges()
# {(0, 1)}
