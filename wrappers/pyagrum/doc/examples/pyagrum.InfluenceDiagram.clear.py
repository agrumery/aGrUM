import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
diag.size()
# 3
diag.clear()
diag.size(), diag.empty()
# (0, True)
