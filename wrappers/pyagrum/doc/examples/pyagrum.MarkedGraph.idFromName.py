import pyagrum as gum

g = gum.fastMarkedGraph("A->B<->C")
g.idFromName("C")
# 2
g.idFromName("Z") is None
# True
