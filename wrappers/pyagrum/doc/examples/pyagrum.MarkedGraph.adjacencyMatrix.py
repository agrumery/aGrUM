import pyagrum as gum

g = gum.fastMarkedGraph("A->B<->C")
g.adjacencyMatrix()
# array([[0, 1, 0],
#        [1, 0, 1],
#        [0, 1, 0]])
