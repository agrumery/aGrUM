import pyagrum as gum

diag = gum.fastID("A->*D->$U;B->$V")
sorted(diag.connectedComponentsList().items())
# [(0, {0, 1, 2}), (1, {3, 4})]
