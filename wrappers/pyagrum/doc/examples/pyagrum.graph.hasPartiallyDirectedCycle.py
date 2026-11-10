import pyagrum as gum
import pyagrum.graph as gg

gg.hasPartiallyDirectedCycle(gum.fastMixedGraph("A->B-C-A"))
# True
gg.hasPartiallyDirectedCycle(gum.fastMixedGraph("A->B-C;A->C"))
# False
