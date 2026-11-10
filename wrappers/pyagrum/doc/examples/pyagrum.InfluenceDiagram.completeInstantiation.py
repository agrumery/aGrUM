import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
print(diag.completeInstantiation())
# <C:0|D:0|U:0>
