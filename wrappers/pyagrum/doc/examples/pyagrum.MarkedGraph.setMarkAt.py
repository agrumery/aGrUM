import pyagrum as gum

g = gum.fastMarkedGraph("A o-> B")
g.setMarkAt(1, 0, gum.EdgeMark.Tail)  # the mark at A becomes a tail
print(g)
# {0<A>,1<B>} , {0 --> 1}
