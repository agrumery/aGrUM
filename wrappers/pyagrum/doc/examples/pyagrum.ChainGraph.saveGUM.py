import pyagrum as gum

cg = gum.fastChainGraph("A->B-C")
cg.saveGUM("cg.jgum")
cg2 = gum.loadGraph("cg.jgum")
type(cg2).__name__, cg2.arcs(), cg2.edges()
# ('ChainGraph', {(0, 1)}, {(1, 2)})
