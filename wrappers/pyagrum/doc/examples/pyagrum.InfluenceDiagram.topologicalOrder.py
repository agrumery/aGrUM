import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
[diag.variable(i).name() for i in diag.topologicalOrder()]
# ['C', 'D', 'U']
