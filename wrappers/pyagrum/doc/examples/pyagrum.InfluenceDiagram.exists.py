import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
diag.exists(diag.idFromName("U"))
# True
diag.exists(42)
# False
