import pyagrum as gum

cg = gum.ChainGraph()
cg.addNodes(3)
# {0, 1, 2}
cg.addArc(0, 1)
cg.addEdge(1, 2)
try:
  cg.addArc(2, 0)  # C->A would close A->B-C into a partially directed cycle
except gum.InvalidPartiallyDirectedCycle:
  print("refused")
cg.arcs()
# {(0, 1)}
