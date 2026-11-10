import pyagrum as gum

diag = gum.fastID("A->*D->$U;B->$V")
diag.connectedComponentsCount()
# 2
