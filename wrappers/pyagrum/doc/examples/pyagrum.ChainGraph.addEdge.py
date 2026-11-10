import pyagrum as gum

cg = gum.fastChainGraph("A->B;C")
cg.addEdge(1, 2)
cg.edges()
# {(1, 2)}
try:
  cg.addEdge(0, 2)  # A->B-C-A would be a partially directed cycle
except gum.InvalidPartiallyDirectedCycle:
  print("refused")
