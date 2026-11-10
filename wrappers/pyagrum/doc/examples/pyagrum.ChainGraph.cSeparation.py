import pyagrum as gum

cg = gum.fastChainGraph("A->B-C;D->C")
cg.cSeparation(0, 3)  # A and D are c-separated
# True
cg.cSeparation(0, 3, 2)  # but not given C
# False
