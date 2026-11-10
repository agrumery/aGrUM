import pyagrum as gum
import pyagrum.graph as gg

g = gum.fastMarkedGraph("A-B->C<->D")
gg.anteriors(g, "C")
# {0, 1}
gg.ancestors(g, "C")
# {1}
gg.anteriors(gum.fastPDAG("A-B->C"), "A")  # A-B-A is not a cycle
# {1}
gg.anteriors(gum.fastMixedGraph("A-B-C-A"), "A")  # a cycle
# {0, 1, 2}
