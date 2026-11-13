import pyagrum as gum

mrf = gum.fastMRF("A--B;C")
deleted = []
listener = mrf.addStructureListener(whenNodeAdded=lambda i, name: None)
listener.setWhenNodeDeleted(lambda i: deleted.append(i))
mrf.erase("C")
deleted
# [2]
