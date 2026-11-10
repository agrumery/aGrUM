import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
diag.cpt("C").fillWith([0.3, 0.7])
diag.cpt("C").names
# ('C',)
diag.cpt("C").tolist()
# [0.3, 0.7]
