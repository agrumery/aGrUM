import pyagrum as gum

pdag = gum.fastPDAG("A->B;A->C;A-D")
pdag.eraseChildren(0)  # the edge A-D is kept
print(pdag)
# {0<A>,1<B>,2<C>,3<D>} , {} , {0--3}
