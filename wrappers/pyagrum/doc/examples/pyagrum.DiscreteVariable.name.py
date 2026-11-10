import pyagrum as gum

bn = gum.fastBN("A->B")
bn.variable(1).name()
# 'B'
