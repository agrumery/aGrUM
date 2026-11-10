import pyagrum as gum

gum.saveGraph(gum.fastPDAG("A->B-C"), "graph.jgum")
g = gum.loadGraph("graph.jgum")
type(g).__name__
# 'PDAG'
sorted(g.arcs()), sorted(g.edges())
# ([(0, 1)], [(1, 2)])
