import pyagrum as gum

g = gum.fastMarkedGraph("A->B;B o-> C")
g.isDefinitelyDirected(0, 1), g.isDefinitelyDirected(1, 0)
# (True, False)
g.isDefinitelyDirected(1, 2)
# False
