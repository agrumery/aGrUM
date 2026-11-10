import pyagrum as gum

diag = gum.fastID("*D1->C->*D2->$U<-D1")
[diag.variable(i).name() for i in diag.decisionOrder()]
# ['D1', 'D2']
