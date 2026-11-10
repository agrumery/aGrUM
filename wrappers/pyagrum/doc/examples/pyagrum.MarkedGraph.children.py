import pyagrum as gum

g = gum.fastMarkedGraph("A->B<->C;B->D")
g.children(1)
# {3}
g.children(2)  # B<->C: C is not a child of B
# set()
