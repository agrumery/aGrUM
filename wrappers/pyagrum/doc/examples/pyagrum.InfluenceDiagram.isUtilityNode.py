import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
diag.isUtilityNode("U")
# True
diag.isUtilityNode("C")
# False
