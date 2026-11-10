import pyagrum as gum

g = gum.fastDiGraph("A->B->C;A->C")
g.adjacencyMatrix().tolist()  # row = tail, column = head
# [[0, 1, 1], [0, 0, 1], [0, 0, 0]]
