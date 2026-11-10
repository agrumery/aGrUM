import pyagrum as gum

cg = gum.fastChainGraph("A->B-C")
dot = cg.toDot()
dot.startswith("digraph")
# True
