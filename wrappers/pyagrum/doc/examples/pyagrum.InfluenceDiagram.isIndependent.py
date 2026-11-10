import pyagrum as gum

diag = gum.fastID("A->B->C;*D->$U<-C")
diag.isIndependent("A", "C", ["B"])
# True
diag.isIndependent("A", "C", [])
# False
