import pyagrum as gum

edge = gum.Edge(5, 2)  # the edge 2-5
edge.other(2)
# 5
edge.other(5)
# 2
