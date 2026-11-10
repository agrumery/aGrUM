import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
diag.parents("U")
# {0, 1}
diag.parents("C")
# set()
