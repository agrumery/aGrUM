import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
diag.setProperty("author", "me")
diag.existsProperty("author")
# True
diag.existsProperty("license")
# False
