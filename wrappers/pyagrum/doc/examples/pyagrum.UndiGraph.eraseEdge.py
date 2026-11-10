import pyagrum as gum

g = gum.fastUndiGraph("A-B-C")
g.eraseEdge(1, 0)
g.edges()
# {(1, 2)}
