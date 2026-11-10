import pyagrum as gum

cpdag = gum.CPDAG.fromDAG(gum.fastDAG("A->B->C->D"))
mpdag = gum.MPDAG.fromCPDAG(cpdag, [(1, 2)])  # B->C imposed, then C->D by the Meek rules
print(mpdag)
# {0<A>,1<B>,2<C>,3<D>} , {2->3,1->2} , {0--1}
