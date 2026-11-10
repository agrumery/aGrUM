import pyagrum as gum

pag = gum.fastPAG("A o-> B<-C")
print(pag)
# {0<A>,1<B>,2<C>} , {0 o-> 1, 1 <-- 2}
