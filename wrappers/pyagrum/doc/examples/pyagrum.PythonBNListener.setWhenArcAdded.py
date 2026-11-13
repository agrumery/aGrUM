import pyagrum as gum

bn = gum.fastBN("A->B;C")
arcs = []
listener = bn.addStructureListener(whenNodeAdded=lambda i, name: None)
listener.setWhenArcAdded(lambda tail, head: arcs.append((tail, head)))
bn.addArc("B", "C")
arcs
# [(1, 2)]
