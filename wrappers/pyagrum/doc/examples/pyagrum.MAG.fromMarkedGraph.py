import pyagrum as gum

mag = gum.MAG.fromMarkedGraph(gum.fastMarkedGraph("A->B<->C;D->C"))
print(mag)
# {0<A>,1<B>,2<C>,3<D>} , {0 --> 1, 2 <-- 3, 1 <-> 2}
try:
  gum.MAG.fromMarkedGraph(gum.fastMarkedGraph("A o-> B"))
except gum.InvalidArgument:
  print("not a MAG: a circle")
