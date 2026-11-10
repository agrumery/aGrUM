import pyagrum as gum
import pyagrum.graph as gg

g = gum.fastDAG("A->B->C;A->D->E->C")
gg.directedPath(g, "A", "C")
# [0, 1, 2]
print(gg.directedPath(g, "C", "A"))
# None
