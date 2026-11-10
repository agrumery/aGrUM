import pyagrum as gum

bn = gum.fastBN("A->B{yes|no|maybe}")
print(bn.variable("B"))
# B:Labelized({yes|no|maybe})
bn.variable(0).name()
# 'A'
