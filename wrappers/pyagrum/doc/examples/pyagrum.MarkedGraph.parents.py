import pyagrum as gum

g = gum.fastMarkedGraph("A->B<-C;B<->D")
g.parents(1)
# {0, 2}
