import pyagrum as gum

bn = gum.fastBN("A->B->C")
deleted = []
listener = bn.addStructureListener(whenNodeAdded=lambda i, name: None)
listener.setWhenNodeDeleted(lambda i: deleted.append(i))
bn.erase("B")
deleted
# [1]
