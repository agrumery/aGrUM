import pyagrum as gum

g = gum.fastMarkedGraph("A->B<->C")
g.saveGUM("graph.jgum")
print(gum.loadGraph("graph.jgum"))
# {0<A>,1<B>,2<C>} , {0 --> 1, 1 <-> 2}
