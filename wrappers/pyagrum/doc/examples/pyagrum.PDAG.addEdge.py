import pyagrum as gum

pdag = gum.fastPDAG("A->B;C")
pdag.addEdge(1, 2)
print(pdag)
# {0<A>,1<B>,2<C>} , {0->1} , {1--2}
