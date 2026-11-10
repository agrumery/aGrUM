import pyagrum as gum
import pyagrum.graph as gg

g = gum.fastUndiGraph("A-B-C;A-C;C-D")
gg.isPerfectEliminationOrdering(g, ["D", "A", "B", "C"])
# True
gg.isPerfectEliminationOrdering(g, ["C", "A", "B", "D"])
# False
