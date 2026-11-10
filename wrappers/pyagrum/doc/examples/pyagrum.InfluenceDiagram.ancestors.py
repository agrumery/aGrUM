import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
sorted(diag.ancestors("U"))
# [0, 1]
sorted(diag.ancestors("C"))
# []
