import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
diag.utility("U").fillWith([10, -5, 0, 20])
diag.utility("U").names
# ('U', 'D', 'C')
diag.utility("U").max()
# 20.0
