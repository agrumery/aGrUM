import pyagrum as gum

pag = gum.fastPAG("A->B;B<->C;C o-o D")
# deprecated: use pyagrum.MAG.fromPAG (a representative MAG)
print(pag.toMixedGraph())
# {0<A>,1<B>,2<C>,3<D>} , {0->1,2->1,1->2} , {2--3}
