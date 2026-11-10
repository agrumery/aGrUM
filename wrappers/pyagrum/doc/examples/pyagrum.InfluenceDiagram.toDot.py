import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
s = diag.toDot()
s.startswith("digraph")
# True
"U" in s
# True
