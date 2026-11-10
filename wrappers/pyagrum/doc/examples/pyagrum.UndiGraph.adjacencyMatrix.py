import pyagrum as gum

g = gum.fastUndiGraph("A-B-C")
g.adjacencyMatrix().tolist()
# [[0, 1, 0], [1, 0, 1], [0, 1, 0]]
