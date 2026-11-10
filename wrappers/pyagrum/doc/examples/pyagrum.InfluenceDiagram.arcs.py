import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
sorted(diag.arcs())
# [(0, 1), (0, 2), (1, 2)]
