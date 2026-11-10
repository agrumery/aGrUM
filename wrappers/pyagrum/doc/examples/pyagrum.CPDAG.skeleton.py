import pyagrum as gum

cpdag = gum.CPDAG.fromDAG(gum.fastDAG("A->B->C;D->C"))
print(cpdag.skeleton())
# {0<A>,1<B>,2<C>,3<D>} , {0--1,2--3,1--2}
