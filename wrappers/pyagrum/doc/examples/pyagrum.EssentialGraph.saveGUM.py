import pyagrum as gum

eg = gum.EssentialGraph(gum.fastBN("A->B<-C"))
eg.saveGUM("eg.jgum")
g = gum.loadGraph("eg.jgum")
type(g).__name__, sorted(g.arcs())
# ('EssentialGraph', [(0, 1), (2, 1)])
