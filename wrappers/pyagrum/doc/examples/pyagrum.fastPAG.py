import pyagrum as gum

pag = gum.fastPAG("A o-> B<-C")
print(pag)
# {0<A>,1<B>,2<C>} , {0 o-> 1, 1 <-- 2}

pag = gum.fastPAG("'Visit Asia' o-> 'X-ray'")  # names between single quotes
sorted(pag.nameFromId(n) for n in pag.nodes())
# ['Visit Asia', 'X-ray']
