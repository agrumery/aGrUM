import pyagrum as gum

bn = gum.fastBN("A->B")
added = []
listener = bn.addStructureListener(whenNodeAdded=lambda i, name: added.append(name))
listener.setWhenNodeAdded(lambda i, name: added.append((i, name)))  # replaces the previous function
bn.add("C")
# 2
added
# [(2, 'C')]
