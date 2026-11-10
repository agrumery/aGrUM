import pyagrum as gum

g = gum.fastUndiGraph("A-B-C")
g.saveGUM("graph.jgum")
g2 = gum.loadGraph("graph.jgum")
type(g2).__name__, sorted(g2.edges())
# ('UndiGraph', [(0, 1), (1, 2)])
