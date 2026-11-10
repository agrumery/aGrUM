import pyagrum as gum
import pyagrum.graph as gg

gg.hasUndirectedCycle(gum.fastUndiGraph("A-B-C-A"))
# True
gg.hasUndirectedCycle(gum.fastMixedGraph("A-B-C->A"))  # the arcs are ignored
# False
