import pyagrum as gum

pdag = gum.fastPDAG("A->B-C;B-D")
pdag.eraseNeighbours(1)  # the arc A->B is kept
print(pdag)
# {0<A>,1<B>,2<C>,3<D>} , {0->1} , {}
