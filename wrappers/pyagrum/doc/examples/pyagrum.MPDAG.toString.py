import pyagrum as gum

mpdag = gum.MPDAG.meekClosure(gum.fastPDAG("A-B->C-D"))
mpdag.toString()
# '{0<A>,1<B>,2<C>,3<D>} , {2->3,1->2} , {0--1}'
