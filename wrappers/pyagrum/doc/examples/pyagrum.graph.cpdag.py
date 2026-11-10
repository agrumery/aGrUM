import pyagrum as gum
import pyagrum.graph as gg

c = gg.cpdag(gum.fastDAG("A->B->C;D->C"))
sorted(c.arcs()), sorted(c.edges())
# ([(1, 2), (3, 2)], [(0, 1)])
