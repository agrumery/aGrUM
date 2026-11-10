import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
diag.saveBIFXML("model.bifxml")
diag2 = gum.InfluenceDiagram()
diag2.loadBIFXML("model.bifxml")
# True
sorted(diag2.names())
# ['C', 'D', 'U']
