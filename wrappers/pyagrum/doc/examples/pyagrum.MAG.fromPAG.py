import pyagrum as gum

pag = gum.fastPAG("A o-> B;B <-o C")
print(gum.MAG.fromPAG(pag))
# {0<A>,1<B>,2<C>} , {0 --> 1, 1 <-- 2}
