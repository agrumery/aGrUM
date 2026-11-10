import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
diag.setProperty("author", "me")
"author" in diag.properties()
# True
diag.property("author")
# 'me'
