import pyagrum as gum

g = gum.fastUndiGraph("A-B-C;D-E;F")
g.connectedComponentsCount()
# 3
