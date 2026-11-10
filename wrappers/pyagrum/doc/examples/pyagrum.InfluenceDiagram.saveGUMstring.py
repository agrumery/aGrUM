import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
s = diag.saveGUMstring()  # a JSON text
diag2 = gum.InfluenceDiagram()
diag2.loadGUMstring(s)
diag2.hasSameStructure(diag)
# True
