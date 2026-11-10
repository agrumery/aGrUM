import pyagrum as gum

cg = gum.fastChainGraph("A->B-C;D->C;C->E")
mag = cg.moralizedAncestralGraph({1})  # the chain component of B and its ancestors
mag.nodes()
# {0, 1, 2, 3}
sorted(mag.edges())
# [(0, 1), (0, 3), (1, 2), (2, 3)]
