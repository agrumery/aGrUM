import pyagrum as gum

mpdag = gum.MPDAG.meekClosure(gum.fastPDAG("A-B->C-D"))
print(mpdag.cpdag())  # no v-structure: every arc is reversible
# {0<A>,1<B>,2<C>,3<D>} , {} , {0--1,2--3,1--2}
