import pyagrum as gum

cpdag = gum.CPDAG.fromDAG(gum.fastDAG("A->B->C;D->C"))
cpdag.adjacencyMatrix()
# array([[0, 1, 0, 0],
#        [1, 0, 1, 0],
#        [0, 0, 0, 0],
#        [0, 0, 1, 0]])
