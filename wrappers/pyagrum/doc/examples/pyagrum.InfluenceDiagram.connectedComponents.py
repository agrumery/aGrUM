import pyagrum as gum

diag = gum.fastID("A->*D->$U;B->$V")
sorted(diag.connectedComponents().items())  # node id: component id
# [(0, 0), (1, 0), (2, 0), (3, 1), (4, 1)]
