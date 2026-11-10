import pyagrum as gum

mpdag = gum.MPDAG.meekClosure(gum.fastPDAG("A-B->C-D"))
print(mpdag)  # C-D oriented by the rule R1
# {0<A>,1<B>,2<C>,3<D>} , {2->3,1->2} , {0--1}
