import pyagrum as gum

g = gum.fastUndiGraph("A-B-C;C-D")
g.eraseNeighbours(2)
g.edges()
# {(0, 1)}
