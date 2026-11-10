import pyagrum as gum

g = gum.fastMarkedGraph("A->B<->C;B<->D")
g.spouses(1)
# {2, 3}
