import pyagrum as gum

cpdag = gum.CPDAG.fromPDAG(gum.fastPDAG("A-B->C<-D"))
print(cpdag)
# {0<A>,1<B>,2<C>,3<D>} , {3->2,1->2} , {0--1}
try:
  gum.CPDAG.fromPDAG(gum.fastPDAG("A->B-C"))  # A->B is not compelled
except gum.InvalidArgument:
  print("not a CPDAG")
