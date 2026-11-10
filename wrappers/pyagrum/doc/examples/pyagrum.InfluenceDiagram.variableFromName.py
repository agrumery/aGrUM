import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
print(diag.variableFromName("C"))
# C:Range([0,1])
