import pyagrum as gum

pdag = gum.fastPDAG("A->B-C")
pdag.saveGUM("pdag.jgum")
print(gum.loadGraph("pdag.jgum"))
# {0<A>,1<B>,2<C>} , {0->1} , {1--2}
