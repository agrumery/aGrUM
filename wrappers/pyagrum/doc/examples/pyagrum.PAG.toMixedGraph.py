import pyagrum as gum

pag = gum.fastPAG("A->B;B<->C;C o-o D")
# deprecated: the circles are lost (C o-o D becomes C-D)
print(pag.toMixedGraph())
# {0<A>,1<B>,2<C>,3<D>} , {0->1,2->1,1->2} , {2--3}

# use instead a MAG represented by the PAG
print(gum.MAG.fromPAG(pag))
# {0<A>,1<B>,2<C>,3<D>} , {0 --> 1, 2 --> 3, 1 <-> 2}
