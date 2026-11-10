import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
diag.nodeId(diag.variable("D"))
# 1
