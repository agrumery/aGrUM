import pyagrum as gum

pdag = gum.fastPDAG("A->B-C<-D;C->E")
# deprecated: use pyagrum.ChainGraph.moralizedAncestralGraph
print(pdag.moralizedAncestralGraph({1}))
# {0<A>,1<B>,2<C>,3<D>} , {0--1,2--3,0--3,1--2}
