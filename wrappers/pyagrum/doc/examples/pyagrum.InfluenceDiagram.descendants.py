import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
sorted(diag.descendants("C"))
# [1, 2]
sorted(diag.descendants("D"))
# [2]
