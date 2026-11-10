import pyagrum as gum

bn = gum.fastBN("A->B->C<-D;A->D")
jt = gum.JunctionTreeGenerator().junctionTree(bn)
sorted(sorted(bn.variable(i).name() for i in jt.clique(c)) for c in jt.nodes())
# [['A', 'B', 'D'], ['B', 'C', 'D']]
