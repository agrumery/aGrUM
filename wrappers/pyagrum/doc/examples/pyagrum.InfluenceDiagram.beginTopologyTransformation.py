import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
diag.beginTopologyTransformation()  # postpone the updates of the tables
diag.eraseArc("C", "U")
diag.addArc("C", "D")
diag.endTopologyTransformation()
sorted(diag.arcs())
# [(0, 1), (1, 2)]
