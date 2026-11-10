import pyagrum as gum

bn = gum.fastBN("A->B{yes|no|maybe}")
v = bn.variableFromName("B")
v.labels()
# ('yes', 'no', 'maybe')
