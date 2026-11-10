import pyagrum as gum

bn = gum.fastBN("A->B")
bn.changeVariableName("A", "Z")
sorted(bn.names())
# ['B', 'Z']
bn.idFromName("Z")
# 0
