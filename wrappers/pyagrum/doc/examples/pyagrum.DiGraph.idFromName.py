import pyagrum as gum

g = gum.fastDiGraph("A->B->C")
g.idFromName("B")
# 1
print(g.idFromName("Z"))
# None
