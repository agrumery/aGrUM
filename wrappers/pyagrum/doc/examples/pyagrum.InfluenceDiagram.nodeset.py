import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
diag.nodeset(["C", "U"])
# {0, 2}
