import pyagrum as gum

g = gum.fastMixedGraph("A->B-C")
g.eraseNode(1)
g.nodes(), g.arcs(), g.edges()
# ({0, 2}, set(), set())
