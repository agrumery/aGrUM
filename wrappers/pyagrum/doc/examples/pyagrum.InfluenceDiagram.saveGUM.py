import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
diag.saveGUM("model.jgum")
gum.loadID("model.jgum").hasSameStructure(diag)
# True
