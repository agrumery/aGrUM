import pyagrum as gum

pdag = gum.fastPDAG("A->B-C<-D")
# deprecated: use pyagrum.ChainGraph.moralGraph
print(pdag.moralGraph())
# {0<A>,1<B>,2<C>,3<D>} , {0--1,2--3,0--3,1--2}
