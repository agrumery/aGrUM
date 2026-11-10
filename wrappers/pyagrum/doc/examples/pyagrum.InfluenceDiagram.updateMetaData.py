import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
diag.updateMetaData()
sorted(diag.properties())
# ['creation', 'lastModification', 'software']
