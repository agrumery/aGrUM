import pyagrum as gum

cg = gum.fastChainGraph("A->B-C;D")
cg.hasMixedReallyOrientedPath(0, 2)  # A->B-C contains an arc
# True
cg.hasMixedReallyOrientedPath(1, 2)  # B-C contains no arc
# False
