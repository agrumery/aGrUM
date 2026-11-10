import pyagrum as gum
import pyagrum.graph as gg

g = gum.fastDAG("A->B->C;A->D")
gg.descendants(g, "A")
# {1, 2, 3}
gg.descendants(g, {"A", "B"})  # B is a descendant of A
# {1, 2, 3}
gg.descendants(gum.fastDiGraph("A->B->A"), "A")  # a directed cycle
# {0, 1}
gg.descendants(gum.fastMarkedGraph("A->B<->C;B->D"), "A")
# {1, 3}
