import pyagrum as gum

bn = gum.fastBN("A->B->C")
mem = bn.memoryFootprint()  # in bytes, depends on the platform
mem > 0
# True
mem < gum.fastBN("A[10]->B[10]->C[10]").memoryFootprint()
# True
