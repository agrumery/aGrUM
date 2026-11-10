import pyagrum as gum

g = gum.fastUndiGraph("A-B-C")
g.idFromName("C")
# 2
print(g.idFromName("Z"))
# None
