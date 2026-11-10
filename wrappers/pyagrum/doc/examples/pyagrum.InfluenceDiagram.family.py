import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
sorted(diag.family("U"))
# [0, 1, 2]
