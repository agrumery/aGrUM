import pyagrum as gum
import pyagrum.graph as gg

g = gum.fastUndiGraph("A-B-C;B-D")
gg.lexBFS(g)
# [0, 1, 3, 2]
gg.lexBFS(g, first=["D"])
# [3, 1, 0, 2]
