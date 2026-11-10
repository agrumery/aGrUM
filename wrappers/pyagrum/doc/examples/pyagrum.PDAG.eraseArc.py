import pyagrum as gum

pdag = gum.fastPDAG("A->B-C")
pdag.eraseArc(0, 1)
print(pdag)
# {0<A>,1<B>,2<C>} , {} , {1--2}
