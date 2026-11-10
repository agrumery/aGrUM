import pyagrum as gum

g = gum.DiGraph.completeGraph(3)
g.nodes()
# {0, 1, 2}
sorted(g.arcs())
# [(0, 1), (0, 2), (1, 2)]
