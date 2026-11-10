import pyagrum as gum

mpdag = gum.MPDAG.meekClosure(gum.fastPDAG("A-B->C;D-E"))
sorted(sorted(c) for c in mpdag.connectedComponentsList().values())
# [[0, 1, 2], [3, 4]]
