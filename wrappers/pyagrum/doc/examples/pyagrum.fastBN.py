import pyagrum as gum

bn = gum.fastBN("A->B[3]<-C{yes|no};B->D[1,5]")
bn.variable("B").labels()
# ('0', '1', '2')
bn.variable("C").labels()
# ('yes', 'no')
bn.variable("D").labels()
# ('1', '2', '3', '4', '5')
sorted(bn.variable(i).name() for i in bn.parents("B"))
# ['A', 'C']
