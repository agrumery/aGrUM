import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
g = mrf.graph()
print(g)
# {0<A>,1<B>,2<C>} , {0--1,1--2}
