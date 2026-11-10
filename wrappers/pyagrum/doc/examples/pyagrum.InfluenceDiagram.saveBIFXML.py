import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
diag.saveBIFXML("model.bifxml")
gum.loadID("model.bifxml").hasSameStructure(diag)
# True
