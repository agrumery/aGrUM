import pyagrum as gum

mpdag = gum.MPDAG.fromPDAG(gum.fastPDAG("A-B->C->D"))
print(mpdag)
# {0<A>,1<B>,2<C>,3<D>} , {2->3,1->2} , {0--1}
try:
  gum.MPDAG.fromPDAG(gum.fastPDAG("A-B->C-D"))  # C-D should be oriented by the Meek rules
except gum.InvalidArgument:
  print("not an MPDAG")
