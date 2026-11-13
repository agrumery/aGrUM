import pyagrum as gum

g = gum.fastMarkedGraph("A->B<->C;C o-o D")
print(g)
# {0<A>,1<B>,2<C>,3<D>} , {0 --> 1, 2 o-o 3, 1 <-> 2}

g = gum.fastMarkedGraph("'A'o->'B'")  # a circle may touch a quoted name
print(g)
# {0<A>,1<B>} , {0 o-> 1}
