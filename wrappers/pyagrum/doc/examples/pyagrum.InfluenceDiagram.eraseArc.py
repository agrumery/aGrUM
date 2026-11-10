import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
diag.eraseArc("C", "U")
sorted(diag.arcs())
# [(0, 1), (1, 2)]
diag.utility("U").names
# ('U', 'D')
