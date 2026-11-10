import pyagrum as gum

diag = gum.fastID("C->*D->$U")
events = []
diag.addStructureListener(
  whenNodeAdded=lambda i, name: events.append(f"node {name} added"),
  whenArcAdded=lambda i, j: events.append(f"arc {i}->{j} added"),
  whenArcDeleted=lambda i, j: events.append(f"arc {i}->{j} deleted"),
)
diag.addChanceNode(gum.LabelizedVariable("E", "E", 2))
# 3
diag.addArc("E", "U")
diag.eraseArc("D", "U")
events
# ['node E added', 'arc 3->2 added', 'arc 1->2 deleted']
