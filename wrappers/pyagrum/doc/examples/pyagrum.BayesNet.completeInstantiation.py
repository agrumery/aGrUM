import pyagrum as gum

bn = gum.fastBN("A->B[3]->C")
inst = bn.completeInstantiation()
print(inst)
# <A:0|B:0|C:0>
inst.domainSize()
# 12
