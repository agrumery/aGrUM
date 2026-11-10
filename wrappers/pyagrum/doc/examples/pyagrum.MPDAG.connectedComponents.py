import pyagrum as gum

mpdag = gum.MPDAG.meekClosure(gum.fastPDAG("A-B->C;D-E"))
cc = mpdag.connectedComponents()
cc[0] == cc[2], cc[0] == cc[3]
# (True, False)
