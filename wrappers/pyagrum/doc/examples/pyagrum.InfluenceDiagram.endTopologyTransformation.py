import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
diag.beginTopologyTransformation()
diag.addArc("C", "D")
diag.erase("U")
diag.endTopologyTransformation()  # the tables are updated now
sorted(diag.names())
# ['C', 'D']
diag.cpt("C").names
# ('C',)
