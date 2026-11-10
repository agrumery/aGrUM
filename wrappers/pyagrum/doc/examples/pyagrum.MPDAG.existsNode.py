import pyagrum as gum

mpdag = gum.MPDAG.meekClosure(gum.fastPDAG("A-B->C-D"))
mpdag.existsNode(3), mpdag.existsNode(4)
# (True, False)
