import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
print(diag.variable("D"))
# D:Range([0,1])
diag.variable(0).name()
# 'C'
