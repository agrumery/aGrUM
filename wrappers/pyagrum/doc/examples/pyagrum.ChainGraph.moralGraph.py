import pyagrum as gum

cg = gum.fastChainGraph("A->B-C;D->C")
# A and D, parents of the chain component {B, C}, are married
sorted(cg.moralGraph().edges())
# [(0, 1), (0, 3), (1, 2), (2, 3)]
