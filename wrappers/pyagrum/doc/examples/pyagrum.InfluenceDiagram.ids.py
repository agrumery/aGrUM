import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
diag.ids(["U", "C"])
# [2, 0]
