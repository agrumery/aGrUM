import pyagrum as gum

cpdag = gum.CPDAG.fromDAG(gum.fastDAG("A->B->C;D->C"))
cpdag.saveGUM("cpdag.jgum")
print(gum.loadGraph("cpdag.jgum"))
# {0<A>,1<B>,2<C>,3<D>} , {3->2,1->2} , {0--1}
