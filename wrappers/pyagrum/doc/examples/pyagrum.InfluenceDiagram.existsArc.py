import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
diag.existsArc("C", "D")
# True
diag.existsArc("D", "C")
# False
