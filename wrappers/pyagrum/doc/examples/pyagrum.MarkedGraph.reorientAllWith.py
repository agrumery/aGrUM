import pyagrum as gum

g = gum.fastMarkedGraph("A->B<->C")
g.reorientAllWith(gum.EdgeMark.Circle)
print(g)
# {0<A>,1<B>,2<C>} , {0 o-o 1, 1 o-o 2}
