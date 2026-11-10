import pyagrum as gum

g = gum.fastDiGraph("A->B->C")
g.saveGUM("graph.jgum")
g2 = gum.loadGraph("graph.jgum")
type(g2).__name__, sorted(g2.arcs())
# ('DiGraph', [(0, 1), (1, 2)])
