import pyagrum as gum

g = gum.fastUndiGraph("A-B-C")
g.hasUndirectedCycle()
# False
g.addEdge(2, 0)
g.hasUndirectedCycle()
# True
