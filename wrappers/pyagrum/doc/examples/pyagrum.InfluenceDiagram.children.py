import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
diag.children("C")
# {1, 2}
diag.children("U")
# set()
