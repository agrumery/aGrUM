import pyagrum as gum

g = gum.fastMarkedGraph("A-B;B-C;B->D")
g.undirectedNeighbours(1)
# {0, 2}
