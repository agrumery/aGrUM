import pyagrum as gum
import pyagrum.graph as gg

order = gg.degeneracyOrdering(gum.fastMixedGraph("A-B-C-A;C->D"))
sorted(order)
# [0, 1, 2, 3]
