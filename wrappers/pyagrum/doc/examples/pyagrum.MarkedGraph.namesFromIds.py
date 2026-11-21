import pyagrum as gum

g = gum.fastMarkedGraph("A->B<->C;D o-> B")
g.namesFromIds(sorted(g.parents(1)))
# ['A']
