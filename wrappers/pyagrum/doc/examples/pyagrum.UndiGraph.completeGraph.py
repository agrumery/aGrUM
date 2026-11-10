import pyagrum as gum

g = gum.UndiGraph.completeGraph(3)
g.nodes()
# {0, 1, 2}
sorted(g.edges())
# [(0, 1), (0, 2), (1, 2)]
