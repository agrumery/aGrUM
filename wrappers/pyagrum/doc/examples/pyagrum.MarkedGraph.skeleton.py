import pyagrum as gum

g = gum.fastMarkedGraph("A->B<->C;C o-o D")
print(g.skeleton())
# {0<A>,1<B>,2<C>,3<D>} , {0--1,2--3,1--2}
