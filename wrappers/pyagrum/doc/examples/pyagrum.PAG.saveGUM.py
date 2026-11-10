import pyagrum as gum

pag = gum.fastPAG("A o-> B;B <-o C")
pag.saveGUM("pag.jgum")
print(gum.loadGraph("pag.jgum"))
# {0<A>,1<B>,2<C>} , {0 o-> 1, 1 <-o 2}
