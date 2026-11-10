import pyagrum as gum

g = gum.fastMarkedGraph("A->B<->C;C o-> D")
g.isDefCollider(0, 1, 2)  # A->B<->C
# True
g.isDefCollider(1, 2, 3)  # B<->C o-> D
# False
