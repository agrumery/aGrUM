import pyagrum as gum

bn = gum.BayesNet.fastPrototype("A->B[3]->C{yes|no}")
sorted(bn.names())
# ['A', 'B', 'C']
print(bn.variable("C"))
# C:Labelized({yes|no})

# 6 is the default domain size of the variables without specification (here A)
bn = gum.BayesNet.fastPrototype("A->B[1,3]<-C{yes|No}->D[2,4]<-E[1,2.5,3.9]", 6)
print(bn.variable("A"))
# A:Range([0,5])
print(bn.variable("B"))
# B:Range([1,3])
print(bn.variable("E"))
# E:Discretized(<[1;2.5[,[2.5;3.9]>)

# a name between single quotes may contain any character
bn = gum.BayesNet.fastPrototype("'X-ray'[3]->'Visit Asia'")
sorted(bn.names())
# ['Visit Asia', 'X-ray']
