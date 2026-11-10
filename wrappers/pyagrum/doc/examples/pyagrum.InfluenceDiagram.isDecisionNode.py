import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
diag.isDecisionNode("D")
# True
diag.isDecisionNode("U")
# False
