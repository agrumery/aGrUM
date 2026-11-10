import pyagrum as gum

pdag = gum.fastPDAG("A->B<-C;B-D")
pdag.eraseParents(1)  # the edge B-D is kept
print(pdag)
# {0<A>,1<B>,2<C>,3<D>} , {} , {1--3}
