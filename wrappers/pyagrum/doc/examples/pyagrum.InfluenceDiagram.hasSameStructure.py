import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
diag.hasSameStructure(gum.fastID("C->*D->$U<-C"))
# True
diag.hasSameStructure(gum.fastID("C->*D->$U"))
# False
