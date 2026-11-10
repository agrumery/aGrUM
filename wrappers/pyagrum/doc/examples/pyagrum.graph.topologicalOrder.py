import pyagrum as gum
import pyagrum.graph as gg

gg.topologicalOrder(gum.fastDAG("C->B->A"))
# [0, 1, 2]
