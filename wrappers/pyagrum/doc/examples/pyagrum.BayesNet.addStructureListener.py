import pyagrum as gum

bn = gum.fastBN("A->B")
events = []
bn.addStructureListener(
  whenNodeAdded=lambda i, name: events.append(f"node {name} added"),
  whenArcAdded=lambda i, j: events.append(f"arc {i}->{j} added"),
)
bn.add("C")
# 2
bn.addArc("B", "C")
events
# ['node C added', 'arc 1->2 added']
