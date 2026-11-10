import pyagrum as gum

mpdag = gum.MPDAG.meekClosure(gum.fastPDAG("A-B->C-D"))
mpdag.adjacencyMatrix()
# array([[0, 1, 0, 0],
#        [1, 0, 1, 0],
#        [0, 0, 0, 1],
#        [0, 0, 0, 0]])
