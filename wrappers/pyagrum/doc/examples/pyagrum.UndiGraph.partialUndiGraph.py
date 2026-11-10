import pyagrum as gum

g = gum.fastUndiGraph("A-B-C;C-D")
sub = g.partialUndiGraph({0, 1, 3})
sorted(sub.nodes())
# [0, 1, 3]
sub.edges()
# {(0, 1)}
