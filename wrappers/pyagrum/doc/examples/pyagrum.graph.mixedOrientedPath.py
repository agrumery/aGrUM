import pyagrum as gum
import pyagrum.graph as gg

g = gum.fastPDAG("A->B-C")
gg.mixedOrientedPath(g, "A", "C")
# [0, 1, 2]
print(gg.mixedOrientedPath(g, "C", "A"))
# None
