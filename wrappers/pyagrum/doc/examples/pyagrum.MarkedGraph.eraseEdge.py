import pyagrum as gum

g = gum.fastMarkedGraph("A->B<->C")
g.eraseEdge(1, 2)
print(g)
# {0<A>,1<B>,2<C>} , {0 --> 1}
