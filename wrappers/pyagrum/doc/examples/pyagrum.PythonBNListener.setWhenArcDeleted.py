import pyagrum as gum

bn = gum.fastBN("A->B->C")
arcs = []
listener = bn.addStructureListener(whenNodeAdded=lambda i, name: None)
listener.setWhenArcDeleted(lambda tail, head: arcs.append((tail, head)))
bn.eraseArc("A", "B")
arcs
# [(0, 1)]
bn.erase("C")  # the arcs of a removed node are removed first
arcs
# [(0, 1), (1, 2)]
