import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
diag.isChanceNode("C")
# True
diag.isChanceNode("D")
# False
