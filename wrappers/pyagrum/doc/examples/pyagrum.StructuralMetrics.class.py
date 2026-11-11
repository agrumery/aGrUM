import pyagrum as gum

ref = gum.fastBN("A->B->C;A->C")
test = gum.fastBN("A->B;C->B;A->C")
comp = gum.StructuralMetrics()
comp.compare(ref, test)
comp.precision()
# 1.0
comp.sid(ref, test)
# 2.0
