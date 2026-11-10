import pyagrum as gum

diag = gum.fastID("A->B->*D->$U<-A")
diag.chanceNodeSize()
# 2
