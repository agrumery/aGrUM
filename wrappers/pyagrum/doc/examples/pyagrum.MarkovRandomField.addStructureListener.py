import pyagrum as gum

mrf = gum.fastMRF("A--B")
events = []
mrf.addStructureListener(
  whenNodeAdded=lambda i, name: events.append(f"node {name} added"),
  whenEdgeAdded=lambda i, j: events.append(f"edge {min(i, j)}--{max(i, j)} added"),
  whenEdgeDeleted=lambda i, j: events.append(f"edge {min(i, j)}--{max(i, j)} deleted"),
)
mrf.add("C", 2)
# 2
mrf.addFactor(["B", "C"])
mrf.eraseFactor(["A", "B"])
events
# ['node C added', 'edge 1--2 added', 'edge 0--1 deleted']
