import pyagrum as gum

cpdag = gum.CPDAG.fromDAG(gum.fastDAG("A->B->C;D->C"))
cpdag.toString()
# '{0<A>,1<B>,2<C>,3<D>} , {1->2,3->2} , {0--1}'
