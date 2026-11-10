import pyagrum as gum
import pyagrum.graph as gg

gg.hasDirectedCycle(gum.fastDiGraph("A->B->C->A"))
# True
gg.hasDirectedCycle(gum.fastMixedGraph("A->B-C-A"))  # the edges are ignored
# False
