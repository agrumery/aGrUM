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

# the second argument is the default domain size of the variables without specification
bn = gum.fastBN("A->B[1,3]<-C{yes|No}->D<-E[1,2.5,3.9]", 6)
print(bn.variable("A"))
# A:Range([0,5])
print(bn.variable("E"))
# E:Discretized(<[1;2.5[,[2.5;3.9]>)

# the CPTs are random, or uniform with uniform=True
bn = gum.fastBN("A->B<-C", uniform=True)
bn.cpt("A").tolist()
# [0.5, 0.5]

# a name between single quotes may contain any character ('' for a quote)
bn = gum.fastBN("'Visit Asia'{yes|no}->'X-ray'[3]<-'l''eau'")
sorted(bn.names())
# ['Visit Asia', 'X-ray', "l'eau"]
