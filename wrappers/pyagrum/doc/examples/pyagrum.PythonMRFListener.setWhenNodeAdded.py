import pyagrum as gum

mrf = gum.fastMRF("A--B")
added = []
listener = mrf.addStructureListener(whenNodeAdded=lambda i, name: added.append(name))
listener.setWhenNodeAdded(lambda i, name: added.append((i, name)))  # replaces the previous function
mrf.add("C")
# 2
added
# [(2, 'C')]
