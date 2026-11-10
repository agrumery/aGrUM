import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
diag.saveGUM("model.jgum")
diag2 = gum.InfluenceDiagram()
diag2.loadGUM("model.jgum")
diag2.hasSameStructure(diag)
# True
