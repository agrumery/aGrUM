import pyagrum as gum
import pyagrum.graph as gg

g = gum.fastMixedGraph("A-B-C;A->D")
gg.undirectedPath(g, "C", "A")
# [2, 1, 0]
print(gg.undirectedPath(g, "A", "D"))  # the arc A->D is ignored
# None
