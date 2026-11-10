import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
diag.toFast()
# 'C[2]->*D[2];C->$U[1];D->U'
