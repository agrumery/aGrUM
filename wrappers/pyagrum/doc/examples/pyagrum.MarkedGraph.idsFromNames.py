import pyagrum as gum

g = gum.fastMarkedGraph("A->B<->C;D o-> B")
g.idsFromNames(("A", "C"))
# (0, 2)
