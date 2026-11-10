import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
sorted(diag.names())
# ['C', 'D', 'U']
