import pyagrum as gum

g = gum.fastMarkedGraph("A->B<->C")
g.eraseNode(1)
print(g)
# {0<A>,2<C>} , {}
