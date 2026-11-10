import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.adjacencyMatrix().tolist()
# [[0, 1, 0], [1, 0, 1], [0, 1, 0]]
