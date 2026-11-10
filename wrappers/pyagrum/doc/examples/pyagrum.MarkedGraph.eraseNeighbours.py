import pyagrum as gum

g = gum.fastMarkedGraph("A->B<->C;B o-o D;C->D")
g.eraseNeighbours(1)
print(g)
# {0<A>,1<B>,2<C>,3<D>} , {2 --> 3}
