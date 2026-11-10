import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
diag.erase("U")
sorted(diag.names())
# ['C', 'D']
sorted(diag.arcs())
# [(0, 1)]
