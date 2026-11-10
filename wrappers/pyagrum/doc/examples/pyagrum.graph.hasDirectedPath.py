import pyagrum as gum
import pyagrum.graph as gg

g = gum.fastPDAG("A->B-C")
gg.hasDirectedPath(g, "A", "B")
# True
gg.hasDirectedPath(g, "A", "C")  # the edge B-C is ignored
# False
