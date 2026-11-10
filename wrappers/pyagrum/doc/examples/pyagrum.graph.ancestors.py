import pyagrum as gum
import pyagrum.graph as gg

g = gum.fastDAG("A->B->C;D->C")
gg.ancestors(g, "C")
# {0, 1, 3}
gg.ancestors(g, ["B", "C"])  # B is an ancestor of C
# {0, 1, 3}
