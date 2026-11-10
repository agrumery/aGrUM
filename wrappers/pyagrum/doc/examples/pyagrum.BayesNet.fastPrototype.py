import pyagrum as gum

bn = gum.BayesNet.fastPrototype("A->B[3]->C{yes|no}")
sorted(bn.names())
# ['A', 'B', 'C']
print(bn.variable("C"))
# C:Labelized({yes|no})
