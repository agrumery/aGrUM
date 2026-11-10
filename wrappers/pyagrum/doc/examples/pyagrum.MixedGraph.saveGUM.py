import pyagrum as gum

g = gum.fastMixedGraph("A->B-C")
g.saveGUM("graph.jgum")
g2 = gum.loadGraph("graph.jgum")
type(g2).__name__, g2.arcs(), g2.edges()
# ('MixedGraph', {(0, 1)}, {(1, 2)})
